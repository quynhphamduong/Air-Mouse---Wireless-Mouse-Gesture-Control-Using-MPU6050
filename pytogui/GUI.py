#GUI.py
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
import pyautogui
import smtplib
import serial
import requests  # For sending data to a cloud API (optional)
import time
import threading
from tkinter import Listbox
from firebase_config import save_feedback_to_firebase 
import json
from pynput.mouse import Listener as MouseListener

def gui_process(shared_values):
    """
    GUI Process for AirMouse with Settings, Connection, and Record tabs.
    """
    def update_multipliers():
        # Update shared values from sliders
        shared_values['x_multiplier'] = x_slider.get()
        shared_values['y_multiplier'] = y_slider.get()
        # Update mouse position label for real-time feedback
        mouse_pos = pyautogui.position()
        mouse_position_label.config(text=f"Mouse Position: {mouse_pos}")
        root.after(100, update_multipliers)  # Refresh values periodically
    
    # Create the main window
    root = tk.Tk()
    root.title("AirMouse")
    root.geometry("900x600")
    root.maxsize(900, 600)  # Set the maximum window size

    # Sidebar on the left
    sidebar = tk.Frame(root, width=120, bg="#ffffff", height=500)
    sidebar.pack(side="left", fill="y")

    # Logo, app's name
    logo_label = tk.Label(sidebar, text="✦ AirMouse.", font=("Arial", 12, "bold"), bg="#ffffff", fg="#333333")
    logo_label.pack(pady=(10, 20))
    
    # Create a Notebook for tabs
    notebook = ttk.Notebook(root)
    notebook.pack(side="left",fill="both", expand=True)

    # CONNECTION Tab
    connection_tab = ttk.Frame(notebook)
    notebook.add(connection_tab, text="Connection")
    #ttk.Label(connection_tab, text="This is the Connection tab").pack(pady=20)

    # SETTINGS Tab
    settings_tab = ttk.Frame(notebook)
    notebook.add(settings_tab, text="Settings")
    
    # Add title for the Settings tab
    #ttk.Label(settings_tab, text="This is the Settings tab", font=("Arial", 16), foreground="gray").pack(pady=10)

    # Add mouse position display
    mouse_position_label = ttk.Label(settings_tab, text="Mouse Position: (0, 0)", font=("Arial", 12), foreground="gray")
    mouse_position_label.pack(pady=10)

    # Add sliders for x_multiplier and y_multiplier
    ttk.Label(settings_tab, text="Adjust X Multiplier:", font=("Arial", 12)).pack(pady=5)
    x_slider = ttk.Scale(settings_tab, from_=1, to=10, orient="horizontal", length=300)
    x_slider.set(1)  # Default value
    x_slider.pack(pady=5)

    ttk.Label(settings_tab, text="Adjust Y Multiplier:", font=("Arial", 12)).pack(pady=5)
    y_slider = ttk.Scale(settings_tab, from_=1, to=10, orient="horizontal", length=300)
    y_slider.set(1)  # Default value
    y_slider.pack(pady=5)

    # RECORD Tab
    record_tab = ttk.Frame(notebook)
    notebook.add(record_tab, text="Record")
    #ttk.Label(record_tab, text="This is the Record tab").pack(pady=20)
    
    # Title
    ttk.Label(record_tab, text="Mouse Action Recorder", font=("Arial", 16)).pack(pady=10)
    
    # Button Start Recording
    start_recording_button = ttk.Button(record_tab, text="Start Recording", command=lambda: start_recording(record_list))
    start_recording_button.pack(pady=5)
    
    # List of records
    ttk.Label(record_tab, text="Recorded Sessions:").pack(pady=5)
    record_listbox = Listbox(record_tab, height=3, font=("Arial", 12))
    record_listbox.pack(pady=5)
    
    # Button Replay
    replay_button = ttk.Button(record_tab, text="Replay", command=lambda: replay_selected(record_listbox))
    replay_button.pack(pady=5)
    
    record_list = []  # Danh sách chứa các bản ghi (tối đa 3 bản ghi)
    recording_data = []  # Dữ liệu ghi hiện tại
    is_recording = False  # Trạng thái ghi
    
    # Label for record details
    record_details_label = ttk.Label(record_tab, text="Select a recording to view details.")
    record_details_label.pack(pady=5)
    
    # Update details when a recording is selected
    record_listbox.bind("<<ListboxSelect>>", lambda e: update_record_details())

    
    #Functions for recording
    def start_recording(record_list):
        global is_recording, recording_data
        if is_recording:
            return  # Nếu đã ghi, bỏ qua
        is_recording = True
        recording_data = []  # Xóa dữ liệu cũ
    
        
    def record_mouse_actions():
        global is_recording, recording_data
        start_time = time.time()
    
        def on_move(x, y):
            if is_recording:
                timestamp = time.time() - start_time
                recording_data.append({"event": "move", "x": x, "y": y, "time": timestamp})
    
        def on_click(x, y, button, pressed):
            if is_recording and pressed:
                timestamp = time.time() - start_time
                recording_data.append({"event": "click", "x": x, "y": y, "time": timestamp})
    
        # Giới hạn thời gian ghi (5 phút)
        def stop_listener():
            if time.time() - start_time > 300:
                return False  # Kết thúc listener sau 5 phút
    
        # Sử dụng listener để theo dõi sự kiện
        with MouseListener(on_move=on_move, on_click=on_click) as listener:
            while is_recording:
                if not is_recording or stop_listener():
                    break
                time.sleep(0.1)
            listener.stop()
    
        # Lưu bản ghi vào danh sách
        if len(record_list) >= 3:  # Xóa bản ghi cũ nhất nếu đã đủ 3 bản ghi
            record_list.pop(0)
        record_list.append(recording_data)
        record_listbox.insert(tk.END, f"Recording {len(record_list)}")  # Cập nhật giao diện

        
    def replay_selected(record_listbox):
        selected_index = record_listbox.curselection()
        if not selected_index:
            messagebox.showwarning("Warning", "Please select a recording to replay!")
            return
    
        selected_record = record_list[selected_index[0]]
    
        # Đếm ngược 3 giây trước khi phát lại
        for i in range(3, 0, -1):
            messagebox.showinfo("Replay", f"Replaying in {i} seconds...")
            time.sleep(1)
    
        # Phát lại bản ghi
        start_time = time.time()
        for action in selected_record:
            elapsed = time.time() - start_time
            delay = action["time"] - elapsed
            if delay > 0:
                time.sleep(delay)
    
            if action["event"] == "move":
                pyautogui.moveTo(action["x"], action["y"])
            elif action["event"] == "click":
                pyautogui.click(action["x"], action["y"])
    


    # Nút Stop Recording
    stop_recording_button = ttk.Button(record_tab, text="Stop Recording", command=lambda: stop_recording())
    stop_recording_button.pack(pady=5)
    
    def stop_recording():
        global is_recording
        is_recording = False
    
    save_button = ttk.Button(record_tab, text="Save to File", command=lambda: save_recording_to_file(record_list))
    save_button.pack(pady=5)
    
    def save_recording_to_file(record_list):
        if not record_list:
            messagebox.showwarning("Warning", "No recordings to save!")
            return
    
        try:
            with open("recordings.json", "w") as f:
                json.dump(record_list, f, indent=4)
            messagebox.showinfo("Success", "Recordings saved to recordings.json")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save recordings: {e}")

    def update_record_details():
        selected_index = record_listbox.curselection()
        if not selected_index:
            record_details_label.config(text="Select a recording to view details.")
            return
    
        selected_record = record_list[selected_index[0]]
        num_events = len(selected_record)
        duration = selected_record[-1]["time"] if selected_record else 0
        record_details_label.config(text=f"Events: {num_events}, Duration: {duration:.2f} seconds")

    # FEEDBACK Tab
    feedback_tab = ttk.Frame(notebook)
    notebook.add(feedback_tab, text="Feedback")
    
    # Title for Feedback Tab
    ttk.Label(feedback_tab, text="Please provide your feedback", font=("Arial", 14, "bold")).pack(pady=10)

    # Create a frame to hold the star buttons
    stars_frame = tk.Frame(feedback_tab)
    stars_frame.pack(pady=10)

    # Star images - you can use your own images for filled and empty stars
    empty_star = "☆"  # Use a Unicode empty star symbol
    filled_star = "★"  # Use a Unicode filled star symbol

    # Variable to hold the current rating
    rating = tk.IntVar()
    rating.set(0)  # Default rating is 0

    # Function to update stars
    def update_stars(selected_rating):
        rating.set(selected_rating)
        for i in range(5):
            if i < selected_rating:
                star_buttons[i].config(text=filled_star, fg="gold")  # Filled star
            else:
                star_buttons[i].config(text=empty_star, fg="gray")  # Empty star

    # Create the star buttons
    star_buttons = []
    for i in range(5):
        button = tk.Button(stars_frame, text=empty_star, font=("Arial", 30), bg="white", bd=0, command=lambda i=i: update_stars(i + 1))
        button.grid(row=0, column=i, padx=5)
        star_buttons.append(button)

    # Display the selected rating
    rating_label = tk.Label(feedback_tab, text="Rating: 0 stars", font=("Arial", 12))
    rating_label.pack(pady=10)

    # Update the rating label based on the user's selection
    def update_rating_label():
        rating_label.config(text=f"Rating: {rating.get()} stars")

    # Update the rating label when the rating changes
    rating.trace("w", lambda *args: update_rating_label())

    # Text box for additional feedback
    ttk.Label(feedback_tab, text="Additional feedback:").pack(pady=5)
    feedback_text = tk.Text(feedback_tab, wrap="word", width=60, height=5, font=("Arial", 12))
    feedback_text.pack(pady=10)

    # Function to send feedback
    def send_feedback():
        try:
            # Lấy rating và feedback từ người dùng
            user_rating = rating.get()
            user_feedback = feedback_text.get("1.0", "end").strip()
    
            if user_rating == 0:
                messagebox.showwarning("Warning", "Please select a rating!")
                return
    
            # Gửi dữ liệu tới Firebase
            save_feedback_to_firebase(user_rating, user_feedback)
            messagebox.showinfo("Success", "Your feedback has been submitted!")
    
            # Reset feedback form
            feedback_text.delete("1.0", "end")
            rating.set(0)
            update_stars(0)
    
        except Exception as e:
            messagebox.showerror("Error", f"Could not send feedback: {e}")
    

    # Button to submit feedback
    submit_feedback_button = ttk.Button(feedback_tab, text="Submit Feedback", command=send_feedback)
    submit_feedback_button.pack(pady=10)

    # HELP Tab
    help_tab = ttk.Frame(notebook)
    notebook.add(help_tab, text="Help")
  
      # Tạo biến để lưu trữ hướng dẫn cho hai ngôn ngữ
    help_text_en = """User Guide for AirMouse:
    
                    version 1.0.0 Dec 2024
                      
        1. Connect the device to your computer via Bluetooth.
        2. Adjust the X and Y multipliers using the sliders in the Settings tab.
        3. Use the mouse to move the cursor.
        4. You can return to the Connection tab to check the connection.
        5. The Record tab helps you log mouse actions.
        6. The Feedback and Help tabs provide feedback and assistance.
        
        Enjoy using AirMouse!"""
    
    help_text_vi = """Hướng dẫn sử dụng AirMouse:
    
                    version 1.0.0  12/2024
                    
        1. Kết nối thiết bị với máy tính qua Bluetooth.
        2. Điều chỉnh hệ số X và Y bằng các thanh trượt trong tab Settings.
        3. Sử dụng chuột để di chuyển con trỏ.
        4. Bạn có thể quay lại tab Connection để kiểm tra kết nối.
        5. Tab Record giúp ghi lại các thao tác chuột.
        6. Tab Feedback và Help cung cấp thông tin phản hồi và trợ giúp.
        
        Chúc bạn sử dụng AirMouse thành công!"""

    # Tạo widget Text cho hướng dẫn sử dụng, cho phép cuộn văn bản
    help_text = tk.Text(help_tab, wrap="word", width=60, height=15, font=("Arial", 12))
    help_text.insert(tk.END, help_text_vi)  # Mặc định hiển thị tiếng Việt
    help_text.config(state=tk.DISABLED)  # Đảm bảo người dùng không thể sửa đổi văn bản
    help_text.pack(pady=20)

    #  có thể sử dụng thêm một thanh cuộn (scrollbar) nếu muốn:
    scrollbar = tk.Scrollbar(help_tab, command=help_text.yview)
    help_text.config(yscrollcommand=scrollbar.set)
    scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
    help_text.pack(pady=10)

    # Chức năng chuyển đổi giữa tiếng Anh và tiếng Việt
    def switch_language_to_english():
        help_text.config(state=tk.NORMAL)  # Enable sửa đổi tạm thời để thay đổi nội dung
        help_text.delete(1.0, tk.END)  # Xóa văn bản hiện tại
        help_text.insert(tk.END, help_text_en)  # Chèn văn bản tiếng Anh
        help_text.config(state=tk.DISABLED)  # Khóa lại nội dung để người dùng không chỉnh sửa

    def switch_language_to_vietnamese():
        help_text.config(state=tk.NORMAL)  # Enable sửa đổi tạm thời để thay đổi nội dung
        help_text.delete(1.0, tk.END)  # Xóa văn bản hiện tại
        help_text.insert(tk.END, help_text_vi)  # Chèn văn bản tiếng Việt
        help_text.config(state=tk.DISABLED)  # Khóa lại nội dung để người dùng không chỉnh sửa

    # Các nút chuyển đổi ngôn ngữ
    language_frame = tk.Frame(help_tab)
    language_frame.pack(pady=10)

    english_btn = tk.Button(language_frame, text="English", font=("Arial", 10), command=switch_language_to_english)
    english_btn.pack(side="left", padx=10)

    vietnamese_btn = tk.Button(language_frame, text="Tiếng Việt", font=("Arial", 10), command=switch_language_to_vietnamese)
    vietnamese_btn.pack(side="left", padx=10)
    
    #Buttons sidebar
    def switch_to_tab(index):
       notebook.select(index)

    buttons = [("Connection", 0), ("Settings", 1), ("Record", 2)]
    for btn_text, index in buttons:
       btn = tk.Button(sidebar, text=f" {btn_text}", font=("Arial", 10), anchor="w", padx=10, borderwidth=0, bg="#ffffff", activebackground="#f0f0f0", command=lambda i=index: switch_to_tab(i))
       btn.pack(fill="x", pady=5)

    # Button Feedback và Help
    feedback = tk.Button(sidebar, text="😊 Feedback", font=("Arial", 10), fg="#ff5c5c", bg="#ffffff", anchor="w", padx=10, borderwidth=0, activebackground="#f0f0f0", command=lambda: switch_to_tab(3))
    feedback.pack(fill="x", pady=5, side="bottom")

    help_btn = tk.Button(sidebar, text="❓ Help", font=("Arial", 10), fg="#3333ff", bg="#ffffff", anchor="w", padx=10, borderwidth=0, activebackground="#f0f0f0", command=lambda: switch_to_tab(4))
    help_btn.pack(fill="x", pady=(0, 10), side="bottom")
    
    
    # Start updating values and run the main loop
    root.after(100, update_multipliers)
    root.mainloop()

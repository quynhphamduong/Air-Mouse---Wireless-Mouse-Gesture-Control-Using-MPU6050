import firebase_admin
from firebase_admin import credentials,db
#pip install firebase-admin


# Init Firebase Admin SDK
cred = credentials.Certificate("C:\\Users\\ADMIN\\Desktop\\pytogui\\airmouse-firebase.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://airmouse-5b955-default-rtdb.asia-southeast1.firebasedatabase.app/'  # Thay 'your-database-name' bằng URL Realtime Database của bạn
})

# Save feedback
def save_feedback_to_firebase(rating, feedback_text):
    try:
        # Prepare feedback data
        feedback_data = {
            "rating": rating,
            "feedback": feedback_text,
        }

        # Add feedback to "feedbacks" branch in Realtime Database
        ref = db.reference('feedbacks')  # Tạo tham chiếu tới nhánh "feedbacks"
        ref.push(feedback_data)  # Sử dụng push() để thêm một node mới
        print("Feedback saved successfully!")
    except Exception as e:
        print(f"Error saving feedback: {e}")
        
# Example of using
#if __name__ == "__main__":
#    save_feedback_to_firebase(5, "This app is amazing!")
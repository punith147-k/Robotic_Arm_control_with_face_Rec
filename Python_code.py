import cv2
import requests

# ESP8266 Web Server URL (Change this to your actual ESP IP)
ESP_IP = "http://192.168.4.1"

# Load Haar cascade for face detection
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

# Open webcam
cap = cv2.VideoCapture(0)

# Frame size
frame_width = int(cap.get(3))
frame_height = int(cap.get(4))

# Initial servo angles
prev_base = 90
prev_elbow = 90

while True:
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Detect faces
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.3, minNeighbors=5, minSize=(30, 30))

    for (x, y, w, h) in faces:
        # Draw rectangle around face
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        # Normalize x and y for servo angles (0-180)
        base_angle = int((x / frame_width) * 180)  # Servo 1 (Base)
        elbow_angle = int((y / frame_height) * 180)  # Servo 2 (Elbow)

        # Smoothen the movement
        base_angle = int((prev_base + base_angle) / 2)
        elbow_angle = int((prev_elbow + elbow_angle) / 2)

        prev_base, prev_elbow = base_angle, elbow_angle  # Store for next frame

        # Send API request to ESP8266
        try:
            response = requests.get(f"{ESP_IP}/move?base={base_angle}&elbow={elbow_angle}")
            print(f"Sent to ESP: base={base_angle}, elbow={elbow_angle}, Response: {response.text}")
        except Exception as e:
            print("Error sending data:", e)

    # Show the frame
    cv2.imshow("Face Tracking", frame)

    # Exit on 'q' key
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
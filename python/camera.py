import cv2
import os
import requests
import time


def capture_and_upload_and_save(api_url, save_dir="captured_images", filename="captured_image.jpg", capture_delay=2):
    """
    Captures an image from the webcam, saves it to a directory, and uploads it to an API.

    Args:
        api_url: The URL of the API endpoint.
        save_dir: The directory to save the image to.
        filename: The name of the saved image file.
        capture_delay: Time in seconds to wait before capturing.
    """

    try:
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            raise IOError("Cannot open webcam")

        time.sleep(capture_delay)

        ret, frame = cap.read()
        if not ret:
            raise IOError("Could not read frame")

        # 1. Save to directory
        if not os.path.exists(save_dir):
            os.makedirs(save_dir)  # Create directory if it doesn't exist

        filepath = os.path.join(save_dir, filename)
        ret, img_encoded = cv2.imencode('.jpg', frame)
        if not ret:
            raise IOError("Could not encode image")

        image_bytes = img_encoded.tobytes()
        with open(filepath, 'wb') as f:
            f.write(image_bytes)
        print(f"Image saved to: {filepath}")

        # 2. Upload to API
        # Use filename here
        files = {'image': (filename, image_bytes, 'image/jpeg')}
        data = {'some_additional_data': 'if_needed'}  # Add any other data

        try:
            response = requests.post(api_url, files=files, data=data)
            response.raise_for_status()  # Raise HTTPError for bad responses (4xx or 5xx)
            print(f"Image uploaded successfully. Response: {response.json()}")

        except requests.exceptions.RequestException as e:
            print(f"Error uploading image: {e}")

    except IOError as e:
        print(f"Error capturing/saving image: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

    finally:
        if 'cap' in locals() and cap.isOpened():
            cap.release()
        cv2.destroyAllWindows()


# Example usage:
api_endpoint = "YOUR_API_ENDPOINT_HERE"  # Replace with your API URL
# The directory where you want to save the image locally
save_directory = "captured_images"
# You can customize the filename here
capture_and_upload_and_save(
    api_endpoint, save_dir=save_directory, filename="my_image.jpg")

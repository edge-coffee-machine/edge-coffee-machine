import pytest
import pyautogui
import pygetwindow as gw
import cv2
import numpy as np
import os
import time

# Offsets to crop the window borders and title bar
border_offset_x = 9
title_bar_height = 40+100
border_offset_bottom = 15

# Paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
REF_DIR = os.path.join(BASE_DIR, "reference_images")
SCREEN_DIR = os.path.join(BASE_DIR, "screenshots")

# Make sure the directories exist
os.makedirs(SCREEN_DIR, exist_ok=True)
os.makedirs(REF_DIR, exist_ok=True)

# Looks for the image "image_name" in the window and clicks on it when found
def click_element(image_name):
    timeout=5
    asset_path = os.path.join(BASE_DIR, "assets", image_name)
    
    if not os.path.exists(asset_path):
        pytest.fail(f"Asset not found: {asset_path}")

    print(f"Looking for: {image_name}...")
    
    # locateCenterOnScreen looks for the image in the screen
    location = None
    start_time = time.time()
    
    while time.time() - start_time < timeout:
        try:
            location = pyautogui.locateCenterOnScreen(asset_path, confidence=0.85)
            if location:
                break
        except pyautogui.ImageNotFoundException:
            pass
        time.sleep(0.5)
        
    if location:
        pyautogui.click(location)
        print(f"Clicked on {location}")
        return True
    else:
        pytest.fail(f"Could not find: {image_name} after {timeout}s")
        return False

# Function to get cropped region of the window to screenshot
def get_window_cropped_region(window):
    crop_left = window.left + border_offset_x
    crop_top = window.top + title_bar_height
    crop_width = window.width - (border_offset_x * 2)
    crop_height = window.height - title_bar_height - border_offset_bottom
    return (crop_left, crop_top, crop_width, crop_height)

# Compares two images in gray scale
def compare_images(img_path_a, img_path_b, threshold=0.99):    
    # Check if files exist
    if not os.path.exists(img_path_b):
        return False, "NO_REF_FOUND"

    img1 = cv2.imread(img_path_a)
    img2 = cv2.imread(img_path_b)
    
    if img1 is None or img2 is None:
        return False, "Error loading images"

    # If dimensions differ, no need to compare
    if img1.shape != img2.shape:
        return False, f"Different dimensions : {img1.shape} vs {img2.shape}"

    # Compute the differences between the images
    diff = cv2.absdiff(img1, img2)
    gray_diff = cv2.cvtColor(diff, cv2.COLOR_BGR2GRAY)
    n_diff_pixels = cv2.countNonZero(gray_diff)
    total_pixels = img1.shape[0] * img1.shape[1]
    similarity = 1 - (n_diff_pixels / total_pixels)
    
    return similarity >= threshold, f"Similarity: {similarity:.4f}"

def screenshot(window, screenshot_name):
    current_shot = os.path.join(SCREEN_DIR, screenshot_name)
    golden_shot = os.path.join(REF_DIR, screenshot_name)
    
    pyautogui.screenshot(current_shot, region=get_window_cropped_region(window))

    match, msg = compare_images(current_shot, golden_shot)

    if not match and msg == "NO_REF_FOUND":
        pytest.fail(f"An screenshot for the main screen has been generated: {current_shot}.\n"
                    f"Review it. If it is correct, copy it to: {golden_shot}\n"
                    f"To serve as golden reference, and rerun the test")

    assert match, f"The UI has changed: {msg}"

def test_main_screen(app_process):
    # Wait a bit for stability
    time.sleep(0.25)

    # Look for the app window
    try:
        # Busca por parte del título. Si falla, ajusta el string "edge_coffe"
        window = gw.getWindowsWithTitle("QUL Primary screen")[0] 
    except IndexError:
        print(f"\n[DEBUG] visible windows: {gw.getAllTitles()}")
        pytest.fail("Could not find the application window. Ensure the app is running and the title is correct.")

    if not window.isActive:
        window.activate()
    
    # Compare main window (initial)
    screenshot(window, "main1.png")
    
    # Enter all drinks
    click_element("all_drinks_button.png")
    time.sleep(0.5)

    # Compare all drinks window
    screenshot(window, "all_drinks.png")

    # Select mocha
    click_element("mocha_button.png")
    time.sleep(0.5)

    # Compare main window (mocha selected)
    screenshot(window, "main2.png")

    # Change coffee amount
    click_element("coffe_slider_button.png")

    # Select make mocha
    click_element("make_mocha_button.png")
    time.sleep(25)

    # Compare main window (mocha first position, changed slider)
    screenshot(window, "main3.png")

    # Switch to another drink and return to check ingredients have been reset
    click_element("espresso_button.png")
    time.sleep(0.2)
    click_element("mocha_button.png")
    time.sleep(0.2)

    # Compare main window (mocha ingredients reset)
    screenshot(window, "main4.png")

    # Change coffee amount
    click_element("coffe_slider_button.png")
    time.sleep(0.2)

    # Reset coffe amount
    click_element("reset_button.png")
    time.sleep(0.2)

    # Compare main window (mocha ingredients reset)
    screenshot(window, "main5.png")
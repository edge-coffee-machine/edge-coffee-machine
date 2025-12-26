import pytest
import pyautogui
import pygetwindow as gw
import cv2
import numpy as np
import os
import time

# Offsets to crop the window borders and title bar
border_offset_x = 9
title_bar_height = 40
border_offset_bottom = 15

# Paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
REF_DIR = os.path.join(BASE_DIR, "reference_images")
SCREEN_DIR = os.path.join(BASE_DIR, "screenshots")

# Make sure the directories exist
os.makedirs(SCREEN_DIR, exist_ok=True)
os.makedirs(REF_DIR, exist_ok=True)

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
    
    print("Tenemos la ventana!")

    if not window.isActive:
        window.activate()
    
    # Get the "Client Area" of the window
    crop_left = window.left + border_offset_x
    crop_top = window.top + title_bar_height
    crop_width = window.width - (border_offset_x * 2)
    crop_height = window.height - title_bar_height - border_offset_bottom
    
    # Screenshot
    screenshot_name = "main.png"
    current_shot = os.path.join(SCREEN_DIR, screenshot_name)
    golden_shot = os.path.join(REF_DIR, screenshot_name)
    
    # region=(left, top, width, height)
    #print(f"Region: left={crop_left}, top={crop_top}, width={crop_width}, height={crop_height}")
    pyautogui.screenshot(current_shot, region=(crop_left, crop_top, crop_width, crop_height))

    # Compare
    match, msg = compare_images(current_shot, golden_shot)
    
    # Lógica de 'Primera vez'
    if not match and msg == "NO_REF_FOUND":
        pytest.fail(f"An screenshot for the main screen has been generated: {current_shot}.\n"
                    f"Review it. If it is correct, copy it to: {golden_shot}\n"
                    f"To serve as golden reference, and rerun the test")

    assert match, f"The UI has changed: {msg}"
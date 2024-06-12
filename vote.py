import pyautogui
from pynput import keyboard

import time

# initialize the bot
pyautogui.FAILSAFE = False
pyautogui.PAUSE = 0.5

# alt + tab to switch to the game
pyautogui.hotkey('alt', 'tab')

def clear_cookies():
    # click pixel (342, 106)
    pyautogui.click(342, 106)

    # click pixel (546, 255)
    pyautogui.click(546, 255)


    # click pixel (1077, 631)
    pyautogui.click(1077, 631)

    # send ctrl + r to clear cookies
    pyautogui.hotkey('ctrl', 'r')
    time.sleep(1)


def vote():
    # press tab 3 times to select the vote 
    for _ in range(3):
        pyautogui.press('tab')

    pyautogui.write('34')

    for _ in range(3):
        pyautogui.press('enter')
        pyautogui.write('33')

    pyautogui.press('enter')
    time.sleep(0.5)


time.sleep(1)

# tant que la touche espace n'est pas pressée
running = True
while running:
    if pyautogui.keyDown('space'):
        running = False
        continue

    # clear cookies
    clear_cookies()

    # press tab 3 times to select the vote
    vote()
    




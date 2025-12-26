import pytest
import subprocess
import time
import os
import signal
import psutil

# This is the path to the executable
APP_REL_PATH = os.path.abspath(os.path.join(
    os.path.dirname(__file__), 
    "../../build/Qt_for_MCUs_2_12_Desktop_32bpp_MINGW-Debug/qmlproject/edge_coffe_machine.exe" 
))

QT_BIN_PATH = r"C:\Qt\6.10.1\mingw_64\bin"

@pytest.fixture(scope="module")
def app_process():
    base_dir = os.path.dirname(__file__)
    app_path = os.path.abspath(os.path.join(base_dir, APP_REL_PATH))
    app_dir = os.path.dirname(app_path)

    if not os.path.exists(app_path):
        pytest.fail(f"No se encuentra el ejecutable en: {app_path}")
    
    env = os.environ.copy()
    if QT_BIN_PATH:
        env["PATH"] = QT_BIN_PATH + os.pathsep + env["PATH"]
    else:
        print("Warning: QT_BIN_PATH no definido, podría fallar si faltan DLLs")
    
    print(f"\n[SETUP] Lanzando: {app_path}")
    print(f"[SETUP] Usando DLLs de: {QT_BIN_PATH}")

    # Launch the app
    process = subprocess.Popen([app_path], cwd=app_dir, env=env)
    
    # Wait for the GUI to start
    time.sleep(1.25)

    if process.poll() is not None:
        pytest.fail(f"La aplicación crasheó (Exit code: {process.returncode}).\n"
                    f"Verifica que la ruta QT_BIN_PATH en conftest.py sea correcta.") 
    
    # Provide the process to the tests
    yield process

    # Kill the process and its children
    print("Cerrando simulación...")
    if process.poll() is None:
        try:
            parent = psutil.Process(process.pid)
            for child in parent.children(recursive=True):
                child.kill()
            parent.kill()
        except psutil.NoSuchProcess:
            pass
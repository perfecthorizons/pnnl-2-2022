Before running the MT SDK Python example, the XDA (xsensdeviceapi)
Python interface needs to be installed using the included XDA wheel file.
Make sure to have the correct version of python and pip are installed on your machine.

Supported versions:
Windows: 3.5.x and 3.6.x
Linux: 3.5.x

1. Make sure to have "wheel" installed on your machine:

pip install wheel

2. Install xsensdeviceapi wheel:

Located in 
Windows: <INSTALL_FOLDER>\MTSDK\Python\x64 or Win32
Linux: <INSTALL_FOLDER>/xsens/python

pip install xsensdeviceapi-<xda version>-cp<Python version>-none-<os type>.whl

For example (MTSDK 4.8.6 wheel for Python 3.5 on Linux):
pip install xsensdeviceapi-4.8.6-cp35-none-linux_x86_64.whl or

For example (MTSDK 4.8.6 wheel for Python 3.6 on Windows):
pip install xsensdeviceapi-4.8.6-cp36-none-win_amd64.whl

3. Now you are ready to run the MT SDK in Python

------------------------------------------------------------------------------------

Important:
On Windows, run examples from command prompt as Administrator to prevent permission issues (some examples use file operations).

PyCharm users, to make the auto-completion work
use:
import xsensdeviceapi.xsensdeviceapi_py35_64 as xda
instead of:
import xsensdeviceapi as xda

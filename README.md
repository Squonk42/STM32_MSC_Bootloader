STM32_MSC_bootloader
=============

## Notice

This software is experimental and a work in progress. Under no circumstances should these files be used in relation to any critical system(s). Use of these files is at your own risk.

## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


## Summary
This is a driverless (no USB drivers needed, even on Windows) USB MSC (Mass Storage Class, aka "USB drive") bootloader for STM32F10X devices. 

It only uses libopencm3 libraries and headers.

This allowed for a very small bootloader 8KB!

## Instructions

 1. git clone https://github.com/Squonk42/STM32_MSC_Bootloader.git
 2. cd STM32_MSC_Bootloader
 3. git submodule update --init # (Only needed once)
 4. make -C libopencm3 # (Only needed once)
 5. make -C src

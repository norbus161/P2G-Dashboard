# P2G-Dashboard

Qt Widgets Dashboard to visualize some radar information of the 24GHz [Infineon Position2Go Radar](https://www.infineon.com/cms/en/product/evaluation-boards/demo-position2go/?redirId=102975). 

This includes: 

- Raw data: I and Q component of both antennas
- Range data: FFT spectrum of both antennas
- Polar plot: Visualizes the targets with range and angle

![dashboard](https://github.com/norbus161/P2G-Dashboard/blob/main/doc/img/dashboard.JPG)

### Used 3rdparty libraries

- [`Persistance1D`](https://github.com/weinkauf/Persistence1D): Extracting and filtering minima and maxima of 1D functions
- [`dj_fft`](https://github.com/jdupuy/dj_fft): FFT processing
- [`ComLib_C_Interface`](https://www.infineon.com/cms/en/tools/landing/infineontoolbox.html?redirId=102781): API for the radar sensor (Included in the Toolbox)
- [`sigwatch`](https://github.com/sijk/qt-unix-signals): Unix signal watcher for Qt

### Dependencies

* `CMake 3.5`
* `Qt 5.15.1`
  * `Qt5Widgets`
  * `Qt5SerialPort`
  * `Qt5Charts`

### Installation of libraries on Raspberry Pi 

```bash
sudo apt-get update
sudo apt-get dist-upgrade
sudo apt-get install cmake qt5-default libqt5serialport5 libqt5serialport5 libqt5serialport5-dev libqt5charts5 libqt5charts5-dev
```

### Build instructions 

Clone the repository:

```bash
git clone https://github.com/norbus161/P2G-Dashboard.git
```

Navigate to the directory and create `build` directory:

```bash
cd P2G-Dashboard && mkdir build
```

Compile the release version for the application:

```bash
cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Serial port permissions (Unix)

Linux: Set up the permissions for accessing the serial port:

```bash
cd .. && sudo cp ./driver/udev/90-p2g-linux.rules /etc/udev/rules.d
sudo udevadm control --reload
sudo reboot -h now
```

### Configuration & execution

```Config.json``` lists some parameters, which can be adjusted. It has to placed in the same director as the executable itself. The ```DspSettings``` will only have an impact to the radar polar plot:

```json
{
    "StatusbarEnabled": false,
    "ToolbarEnabled": false,	
	
	"DspSettings":{
		"RangeMovingAverageFilterLength": 5,
	        "MinRange": 20,
		"MaxRange": 1000,
		"MinSpeed": 0,
		"MaxSpeed": 4,
		"SpeedThreshold": 0,
		"RangeThreshold": 100,
		"Tracking": false,
		"NumberOfTracks": 5,
		"MedianFilterDepth": 5,
		"MTIFilterSelection": false,
		"MTIFilterWeight": 100
	}
}
```



Execute the application:

```bash
./P2G-Dashboard
```



### Todos

- [ ] Rangeplot: show maxima labels only for the antenna (1 OR 2) with global maxima
- [ ] Put Range calculation into signal processor class
- [ ] Radarthread: automatic reconnect 
- [ ] Save option for dynamic dsp configuration
- [ ] **Refactor code**


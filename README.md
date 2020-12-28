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

### Serial port & execution

Linux: Set up the permissions for accessing the serial port:

```bash
cd .. && sudo cp ./driver/udev/90-p2g-linux.rules /etc/udev/rules.d
sudo udevadm control --reload
sudo reboot -h now
```

Execute the application:

```bash
./P2G-Dashboard
```



### Todos

- [ ] Rangeplot: show maxima labels only for the antenna (1 OR 2) with global maxima
- [ ] Put Range calculation into signal processcor
- [ ] Radarthread: automatic reconnect 
- [ ] Settings: add input validator for ranges of settings
- [ ] Settings: units for the settings
- [ ] Connect only to specified FW version
- [ ] Memory leak check
- [ ] **Refactor code**


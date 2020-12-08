# P2G-Dashboard

Qt Widgets GUI to visualize some radar information of the [Infineon Position2Go-Kit](https://www.infineon.com/cms/en/product/evaluation-boards/demo-position2go/?redirId=102975). 
This includes: 

- Raw data: I and Q component of both antennas
- Range data: FFT spectrum of both antennas
- Polar plot: Visualizes the targets with range and angle

![dashboard](https://github.com/norbus161/P2G-Dashboard/blob/main/doc/img/dashboard.JPG)

### Dependencies

* `CMake 3.5`
* `Qt 5.15.1`
  * `QtCharts`

### Build instructions 

Clone the repository:

- `git clone https://github.com/norbus161/P2G-Dashboard.git`

Navigate to the directory and create `build` directory:

- `cd P2G-Dashboard && mkdir build`

Compile the release version for the application:

- `cmake -DCMAKE_BUILD_TYPE=Release ..`
- `make` 

Execute the application:

- `./P2G-Dashboard`

### Todos

- [ ] Rangeplot: show maxima labels only for the antenna (1 OR 2) with global maxima
- [ ] Polarplot: show visible angle of the radar within the polarplot
- [ ] Polarplot: 180 degrees instead of 360 degrees
- [ ] Create Ini-File to configure target detection, detection of extrema and arrangement of  layouts.
- [x] **Create udev rule for serial port** 
- [x] **Create installation guide**
- [ ] **Refactor code**
- [x] **Compile static linked qt release version for Ubuntu 18.04**
- [x] **Deliver executable with install guide**

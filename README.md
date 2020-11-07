# P2G-Dashboard

Qt Widgets GUI to visualize some radar information of the Infineon Position2Go-Kit. 
This includes: 

- Raw data: I and Q component of both antennas
- Range data: FFT spectrum of both antennas
- Polar plot: Visualizes the targets with range and angle

![dashboard](C:\Users\Patrick\Desktop\P2G-Gui\doc\img\dashboard.JPG)

### Dependencies

* `CMake 3.5`
* `Qt 5.15.1`
  * `QtCharts`

### Build instructions

Clone the repository with SSH or HTTPS:

`git clone https://github.com/norbus161/P2G-Gui.git`

Navigate to the directory and create `build` directory

`cd P2G-Gui && mkdir build`

Call the CMake-command and compile the application

`cmake .. && make`

### Todos

- [x] Rangeplot automatic scaling for y-axes
- [x] Rangeplot fix decimals for maxima labels
- [x] Polarplot 
- [ ] Refactor code
- [ ] Compile release version for Ubuntu 18.04
- [ ] Deliver executable with dependencies
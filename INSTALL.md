### Requirements on Ubuntu 18.04

#### Dependencies:

`sudo apt-get update`

`sudo apt-get install libxcb-xinput0 `

`sudo apt-get install libxcb-xinerama0`

Check with `ldd ./P2G-Dashboard` that all dependencies are met

#### Output (best case):

- ​	`linux-vdso.so.1 (0x00007ffcf2d60000)`
- ​	`libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f755ea08000)`
- ​	`libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f755e67f000)`
- ​	`libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f755e2e1000)`
- ​	`libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f755e0c9000)`
- ​	`libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f755dcd8000)`
- ​	`libxcb-glx.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-glx.so.0 (0x00007f755dabd000)`
- ​	`libX11-xcb.so.1 => /usr/lib/x86_64-linux-gnu/libX11-xcb.so.1 (0x00007f755d8bb000)`
- ​	`libxcb-icccm.so.4 => /usr/lib/x86_64-linux-gnu/libxcb-icccm.so.4 (0x00007f755d6b6000)`
- ​	`libxcb-image.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-image.so.0 (0x00007f755d4b1000)`
- ​	`libxcb-shm.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-shm.so.0 (0x00007f755d2ae000)`
- ​	`libxcb-keysyms.so.1 => /usr/lib/x86_64-linux-gnu/libxcb-keysyms.so.1 (0x00007f755d0ab000)`
- ​	`libxcb-randr.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-randr.so.0 (0x00007f755ce9b000)`
- ​	`libxcb-render-util.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-render-util.so.0 (0x00007f755cc97000)`
- ​	`libxcb-render.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-render.so.0 (0x00007f755ca8a000)`
- ​	`libxcb-shape.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-shape.so.0 (0x00007f755c886000)`
- ​	`libxcb-sync.so.1 => /usr/lib/x86_64-linux-gnu/libxcb-sync.so.1 (0x00007f755c67f000)`
- ​	`libxcb-xfixes.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-xfixes.so.0 (0x00007f755c477000)`
- ​	`libxcb-xinerama.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-xinerama.so.0 (0x00007f755c274000)`
- ​	`libxcb-xkb.so.1 => /usr/lib/x86_64-linux-gnu/libxcb-xkb.so.1 (0x00007f755c058000)`
- ​	`libxcb-xinput.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-xinput.so.0 (0x00007f755be37000)`
- ​	`libxcb.so.1 => /usr/lib/x86_64-linux-gnu/libxcb.so.1 (0x00007f755bc0f000)`
- ​	`libxkbcommon-x11.so.0 => /usr/lib/x86_64-linux-gnu/libxkbcommon-x11.so.0 (0x00007f755ba07000)`
- ​	`libX11.so.6 => /usr/lib/x86_64-linux-gnu/libX11.so.6 (0x00007f755b6cf000)`
- ​	`libfontconfig.so.1 => /usr/lib/x86_64-linux-gnu/libfontconfig.so.1 (0x00007f755b48a000)`
- ​	`libfreetype.so.6 => /usr/lib/x86_64-linux-gnu/libfreetype.so.6 (0x00007f755b1d6000)`
- ​	`libxkbcommon.so.0 => /usr/lib/x86_64-linux-gnu/libxkbcommon.so.0 (0x00007f755af97000)`
- ​	`libGL.so.1 => /usr/lib/x86_64-linux-gnu/libGL.so.1 (0x00007f755ad0b000)`
- ​	`libpng16.so.16 => /usr/lib/x86_64-linux-gnu/libpng16.so.16 (0x00007f755aad9000)`
- ​	`libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f755a8bc000)`
- ​	`libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f755a6b8000)`
- ​	`/lib64/ld-linux-x86-64.so.2 (0x00007f755f96d000)`
- ​	`libxcb-util.so.1 => /usr/lib/x86_64-linux-gnu/libxcb-util.so.1 (0x00007f755a4b2000)`
- ​	`libXau.so.6 => /usr/lib/x86_64-linux-gnu/libXau.so.6 (0x00007f755a2ae000)`
- ​	`libXdmcp.so.6 => /usr/lib/x86_64-linux-gnu/libXdmcp.so.6 (0x00007f755a0a8000)`
- ​	`libexpat.so.1 => /lib/x86_64-linux-gnu/libexpat.so.1 (0x00007f7559e76000)`
- ​	`libGLX.so.0 => /usr/lib/x86_64-linux-gnu/libGLX.so.0 (0x00007f7559c45000)`
- ​	`libGLdispatch.so.0 => /usr/lib/x86_64-linux-gnu/libGLdispatch.so.0 (0x00007f755998f000)`
- ​	`libbsd.so.0 => /lib/x86_64-linux-gnu/libbsd.so.0 (0x00007f755977a000)`
- ​	`librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007f7559572000)`



#### Permissions:

`sudo cp ./driver/udev/10-p2g-ubuntu.rules /etc/udev/rules.d`

`sudo udevadm control --reload`

`sudo reboot -h now`

#### Execute the application:

`./P2G-Dashboard` 






# Projection mapping tool for 2 displays

## Get started

### Download openFrameworks

- download openFrameworks from [openFrameworks official site](http://www.openframeworks.cc/download) 
- unzip zip file

### Adding addons

- unzip added_addons.zip
- copy to [of_directory]/addons

### Copy application files 

- copy application files to [of_direcotry]/apps/myApps/[your app directory]

### Build & Run

- Open project file and build

## Usage

### Requirements

You need 2 projectors for projection mapping

- Projector resolution and PC monitor resolution must be same.
	- ex. 1920 x 1080

### 1. Edit settings file (data/system_settings.xml)

- screenWidth, screenHeight: Projector resolution
- captureWidth, captureHeight: Capturing resolution

### 2. Prepare input device

- You need USB camera

### 3. Run the application

### 4. Select video capture device

- select input device from dropdown list

### 5. Select effective video area

- drag 2 manipulators of corners

### 6. Mapping

key bindings and mouse control

1. Select camera
	- '1': select left camera
	- '2': select right camera
2. Adjust mapping 
	- drag 4 manipulators of corners

'0' or Backspace to go back to device select mode

### Save parameter

key bindings

- 's': save current settings
- 'l': load saved settings

### Other key bindings

- 'h': show help
- 'd': toggle frame-rate info
- Esc: quit

## Testing environments

- v0.8.4 + Windows7 + Code::Blocks
- v0.8.4 + Mac + Xcode6

## Precomiled binary

[nesicpmtest.exe](http://momonoki.blob.core.windows.net/app/nesicpm/NesicPMTest.exe) (Win7)
# ChibiOS Application Setup

This guide explains how to properly set up and build your own ChibiOS **app** folder inside the ChibiOS workspace.



## 📂 1. Navigate to Your App Folder

```bash
cd ~/chibios-workspace/ChibiOS/app
ls -la
````

👉 You should see:

```
cfg/  main.c  Makefile  readme.txt  (and other files from the demo)
```

---

## 📝 2. Update the Makefile

The most important change is to update the **ChibiOS path**, since your app is now directly inside the ChibiOS root directory.

```bash
nano Makefile
```

### Change this line (near the top):

```make
CHIBIOS = ../..
```

➡️ To:

```make
CHIBIOS = ..
```

### Optionally change the project name:

```make
# Original
PROJECT = ch

# Custom
PROJECT = my-app
```

---

## 🛠️ 3. Test the Build

```bash
# Clean and build
make clean
make

# Check build output
ls -la build/
```

👉 You should see `.elf`, `.bin`, and `.hex` files with your project name.

---

## ⚡ 4. Test Connection and Flash

```bash
# Test ST-LINK connection
st-info --probe

# Flash the app if the connection works
st-flash write build/ch.bin 0x08000000
```

> ⚠️ Modify the **application name** before flashing.

---

## 📁 5. Final Project Structure

```
~/chibios-workspace/ChibiOS/
├── app/                    # Your application folder
│   ├── cfg/
│   │   ├── chconf.h
│   │   ├── halconf.h
│   │   └── mcuconf.h
│   ├── main.c              # Your main application
│   ├── Makefile            # Build configuration
│   └── build/              # Build output
├── demos/                  # Original demos
├── os/                     # ChibiOS source
└── ...
```

---

## 🔑 Key Point

Make sure this line is set correctly in your **Makefile**:

```make
CHIBIOS = ..
```

(one level up instead of two levels up from the original demo location).

---

✅ Now try building with:

```bash
make
```

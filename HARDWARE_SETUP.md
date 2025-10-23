# Hardware Setup Guide

## Seeed Xiao ESP32S3 Sense Pinout

```
                    ┌─────────┐
                    │ USB-C   │
                    └────┬────┘
         ┌───────────────┴───────────────┐
         │  Seeed Xiao ESP32S3 Sense     │
         │                                │
    D0   │ GPIO1  (BUTTON)         3V3   │ 3.3V Out
    D1   │ GPIO2  (BUZZER)         GND   │ Ground
    D2   │ GPIO3                   VBUS  │ 5V (USB)
    D3   │ GPIO4                   GPIO10│
    D4   │ GPIO5                   GPIO9 │
    D5   │ GPIO6                   GPIO8 │
    D6   │ GPIO43                  GPIO7 │
    D7   │ GPIO44                  GPIO21│ (LED)
    D8   │ GPIO19/20               5V    │ 5V Out
    D9   │ GPIO18              RX  GPIO42│
    D10  │ GPIO17              TX  GPIO41│
         │                                │
         │        [CAMERA MODULE]         │
         └────────────────────────────────┘
```

## Component Wiring

### 1. Push Button (Manual Trigger)

```
Button Pin 1 ──────┬───────> GPIO1 (D0)
                   │
Button Pin 2 ──────┴───────> GND
```

- Use a normally-open push button
- Internal pull-up resistor is enabled in code
- Press to trigger manual capture

### 2. Buzzer (Audio Feedback)

#### For Passive Buzzer:
```
Buzzer (+) ────────────────> GPIO2 (D1)
Buzzer (-) ────────────────> GND
```

#### For Active Buzzer:
```
                     ┌─ Transistor Circuit ─┐
GPIO2 ──[1kΩ]──> Base (2N2222)
                 Collector ──> Buzzer (+)
                 Emitter ────> GND
Buzzer (-) ──────────────────> 3.3V
```

### 3. LED Status Indicator

The built-in LED on GPIO21 is automatically configured. No external wiring needed.

### 4. Power Options

#### USB Power (Development):
```
USB-C Cable ────> ESP32S3 USB Port
                  (Provides 5V power and serial communication)
```

#### Battery Power (Portable):
```
Battery (+) ────> 5V pin
Battery (-) ────> GND pin

Recommended: 3.7V LiPo with 5V boost converter
Or: USB power bank with USB-C output
```

## Complete Wiring Diagram (ASCII)

```
                          ESP32S3 Sense
                       ┌─────────────────┐
                       │                 │
    ┌─[Button]─┬──────┤ GPIO1 (D0)      │
    │          └──────┤ GND             │
    │                 │                 │
    ┌─[Buzzer+]──────┤ GPIO2 (D1)      │
    └─[Buzzer-]──────┤ GND             │
                      │                 │
    ┌─[Battery+]─────┤ 5V              │
    └─[Battery-]─────┤ GND             │
                      │                 │
                      │ GPIO21          │─── Built-in LED
                      │                 │
                      │ [CAMERA]        │─── Built-in Camera
                      │                 │
                      └─────────────────┘
```

## Assembly Steps

### Step 1: Prepare Components
1. Seeed Xiao ESP32S3 Sense board
2. Breadboard or prototype PCB
3. Push button (tactile switch)
4. Buzzer (3-5V compatible)
5. Jumper wires
6. Optional: enclosure/case

### Step 2: Connect Button
1. Place button on breadboard
2. Connect one pin to GPIO1 (D0) with jumper wire
3. Connect other pin to GND rail

### Step 3: Connect Buzzer
1. Identify buzzer polarity (+/-)
2. Connect positive lead to GPIO2 (D1)
3. Connect negative lead to GND rail

### Step 4: Power Connection
**For testing:**
- Connect USB-C cable to computer
- No additional power wiring needed

**For deployment:**
- Connect battery positive to 5V pin
- Connect battery negative to GND pin
- Ensure battery provides 5V regulated output

### Step 5: Verify Connections
Before powering on:
- ✓ Check all connections are secure
- ✓ Verify no short circuits between power rails
- ✓ Ensure buzzer polarity is correct
- ✓ Confirm button is properly connected

## Testing the Hardware

### Initial Power-On Test
1. Connect USB cable
2. Watch for LED to turn on
3. Should hear two startup beeps (1000Hz, 1500Hz)
4. Check Serial Monitor (115200 baud) for messages

### Button Test
1. Press the button
2. Should hear short beep (1500Hz)
3. LED should turn off briefly (capturing)
4. Serial Monitor shows "Button pressed"

### Camera Test
1. System should auto-capture every 5 seconds
2. LED turns off during capture
3. Serial Monitor shows "Capturing image..."

## Enclosure Recommendations

### 3D Printed Case
- Design with camera lens opening
- Button accessible on side
- Buzzer sound holes
- USB-C port access
- Mounting points for lanyard/clip

### Commercial Enclosure
- Small project box (60x40x20mm minimum)
- Drill holes for:
  - Camera lens (8mm diameter)
  - Buzzer (multiple small holes)
  - Button (6mm diameter)
  - USB-C port (rectangular cutout)

## Mounting for Users

### Wearable Options
1. **Chest Mount**: Clip or lanyard around neck
2. **Wrist Mount**: Velcro strap attachment
3. **Belt Clip**: Waist-level mounting
4. **Backpack Strap**: Side pocket attachment

### Positioning Tips
- Camera should face forward
- Angle slightly downward (15-30°) for ground objects
- Keep lens clean and unobstructed
- Ensure button is easily accessible

## Safety Considerations

### Electrical Safety
- ✓ Use proper insulation for all connections
- ✓ Avoid exposing bare wires
- ✓ Use appropriate voltage regulator for battery
- ✓ Include over-current protection

### Mechanical Safety
- ✓ Secure all components to prevent movement
- ✓ Use smooth enclosure without sharp edges
- ✓ Ensure device is lightweight for comfort
- ✓ Add lanyard attachment to prevent drops

### User Safety
- ⚠ Device is an aid, not a replacement for other tools
- ⚠ Always use with traditional mobility aids
- ⚠ Test in safe environment before relying on it
- ⚠ Battery should be safely enclosed

## Troubleshooting Hardware Issues

### No Power
- Check USB cable connection
- Verify battery voltage (should be ~5V)
- Test with different power source

### Camera Not Working
- Verify camera ribbon cable connection
- Check for physical damage to camera
- Ensure correct board variant (ESP32S3 **Sense**)

### No Audio Feedback
- Check buzzer polarity
- Test buzzer with multimeter
- Verify GPIO2 connection
- Try different buzzer

### Button Not Responding
- Check button connections
- Verify button is normally-open type
- Test button with multimeter for continuity
- Check Serial Monitor for debug messages

## Optional Enhancements

### External Power Switch
```
Battery (+) ──[Switch]──> 5V pin
Battery (-) ──────────> GND pin
```

### Status LED (Additional)
```
GPIO3 ──[220Ω resistor]──> LED (+)
LED (-) ──────────────────> GND
```

### External Speaker (Better Audio)
```
GPIO2 ──[Audio Amplifier]──> Speaker
```

## Bill of Materials (BOM)

| Component | Quantity | Approx. Cost |
|-----------|----------|--------------|
| Seeed Xiao ESP32S3 Sense | 1 | $15 |
| Tactile Push Button | 1 | $0.50 |
| Passive Buzzer 5V | 1 | $1 |
| Jumper Wires | 10 | $2 |
| USB-C Cable | 1 | $3 |
| Breadboard (optional) | 1 | $3 |
| Battery Pack (optional) | 1 | $10 |
| **Total** | | **~$35** |

## Maintenance

### Regular Checks
- Clean camera lens weekly with soft cloth
- Check connections for looseness
- Verify battery charge level
- Test button functionality

### Battery Maintenance
- Recharge when voltage drops below 3.5V
- Store at room temperature
- Replace after 300-500 charge cycles
- Never over-discharge below 3.0V

## Warranty and Support

For hardware issues:
1. Check all connections first
2. Review troubleshooting guide
3. Test individual components
4. Consult Seeed Studio documentation
5. Create GitHub issue if problem persists

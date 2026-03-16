# ProStreet Most Wanted Physics

Experiment for Need for Speed: ProStreet that converts the physics and handling model from Most Wanted and Carbon, including rechargeable NOS and the ability to use speedbreaker

Massive thanks to everyone involved with [dbalatoni13's MW reverse engineering project](https://github.com/dbalatoni13/nfsmw), this mod was heavily based off of it.

## Installation

- Make sure you have v1.1 of the game, as this is the only version this plugin is compatible with. (exe size of 3765248 or 28739656 bytes)
- Plop the files into your game folder.
- Enjoy, nya~ :3

## Useful info

- Fine tuning has been entirely redone, many options might change car performance in different ways than before, and the following tunings do not affect anything anymore: Brake Pressure, Camber, Caster, Start/End Boost, Tire Pressure, Handbrake Pressure, Toe, Cam Timing
- FOR DEVELOPERS: Almost all VLT values have been replaced! For editing car handling, edit the files in `CarDataDump` instead, except for these values: `ASPECT_RATIO`, `FRONT_AXLE`, `MASS`, `RIM_SIZE`, `SECTION_WIDTH`, `TRACK_WIDTH`, `WHEEL_BASE`
- Manual transmission no longer gives you an advantage, and the clutch button has been entirely removed
- Nitrous no longer fires in bursts, you need to hold the NOS button down to use it, and it regenerates just like MW
- Perfect starts have been re-added, though there's no visual indication of the target RPM so you'll have to go by feel
- Speedbreaker has been re-added, bound to the X key on the keyboard and the X button on an Xbox controller

## Known issues

- Damage no longer affects car handling
- Drafting no longer works
- Drift and drag events revert to the vanilla game's handling, as burnouts, wheelies and drift scoring currently don't work with MW physics
- Skidmarks and tire smoke aren't 100% accurate

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process. 

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common), [nya-common-nfsps](https://github.com/gaycoderprincess/nya-common-nfsps) and [CwoeeMenuLib](https://github.com/gaycoderprincess/CwoeeMenuLib) to folders next to this one, so they can be found.

Required packages: `mingw-w64-gcc`

You should be able to build the project now in CLion.

## Handling List

- Acura Integra - Mercedes-Benz CLK 500 (Most Wanted)
- Acura Integra Type R - Mercedes-Benz CLK 500 (Most Wanted)
- Acura NSX - Mazda RX-7 (Most Wanted)
- Acura RSX - Mitsubishi Eclipse (Most Wanted)
- Aston Martin DB9 - Aston Martin DB9 (Most Wanted)	
- Aston Martin DBR9 - Aston Martin DB9 (Most Wanted)	
- Audi R8 - Audi R8 (Carbon)
- Audi RS4 - Audi A4 (Most Wanted)
- Audi S3 - Audi A3 (Most Wanted)
- Audi S4 - Audi A4 (Most Wanted)
- Audi TT - Audi TT (Most Wanted)
- BMW M3 E46 - BMW M3 GTR (Most Wanted)
- BMW M3 E92 - BMW M3 GTR (Most Wanted)
- BMW Z4 M Coupe - Nissan 350Z (Carbon)
- Bugatti Veyron - Audi R8 (Carbon)
- Cadillac CTS-V - Cadillac CTS (Most Wanted)
- Chevrolet Camaro Concept - Chevrolet Camaro Concept (Carbon)
- Chevrolet Camaro SS - Chevrolet Camaro SS (Most Wanted)
- Chevrolet Chevelle SS - Chevrolet Chevelle SS (Carbon)
- Chevrolet Cobalt SS - Chevrolet Cobalt SS (Most Wanted)
- Chevrolet Corvette '67 - Chevrolet Corvette C6 (Most Wanted)
- Chevrolet Corvette Z06 - Chevrolet Corvette C6.R (Most Wanted)
- Dodge Challenger Concept - Dodge Challenger Concept (Carbon)
- Dodge Challenger R/T - Dodge Challenger R/T (Carbon)
- Dodge Charger R/T - Dodge Charger R/T (Carbon)
- Dodge Viper SRT10 - Dodge Viper SRT10 (Most Wanted)
- Ford Escort RS Cosworth - Subaru Impreza WRX STI (Most Wanted)
- Ford Focus ST - Fiat Punto (Most Wanted)
- Ford GT - Ford GT (Most Wanted)
- Ford Mustang GT '03 - Ford Mustang GT (Most Wanted)
- Ford Mustang GT - Ford Mustang GT (Most Wanted)
- Ford Shelby GT500 '06 - Ford Shelby GT500 '06 (Carbon)
- Ford Shelby GT500 '67 - Ford Shelby GT500 '67 (Carbon)
- Honda Civic - Fiat Punto (Most Wanted)
- Honda Civic Si - Chevrolet Cobalt SS (Most Wanted)
- Honda S2000 - Mazda RX-7 (Most Wanted)
- Infiniti G35 - Infiniti G35 (Carbon)
- Koenigsegg CCX - Koenigsegg CCX (Carbon)
- Lamborghini Gallardo Superleggera - Lamborghini Gallardo (Most Wanted)
- Lamborghini Murcielago LP640 - Lamborghini Murcielago (Most Wanted)
- Lancia Delta HF - Fiat Punto (Most Wanted)
- Lexus IS350 - Lexus IS300 (Most Wanted)
- Lotus Elise - Lotus Elise (Most Wanted)
- Mazda RX-7 - Mazda RX-7 (Most Wanted)
- Mazda RX-8 - Mazda RX-8 (Most Wanted)
- McLaren F1 - Chevrolet Corvette C6.R (Most Wanted)
- Mercedes-Benz SL65 - Mercedes-Benz SL65 (Most Wanted)
- Mitsubishi Eclipse - Mitsubishi Eclipse (Most Wanted)
- Mitsubishi Lancer EVOLUTION IX - Mitsubishi Lancer Evolution VIII (Most Wanted)
- Mitsubishi Lancer EVOLUTION X - Mitsubishi Lancer Evolution VIII (Most Wanted)
- Nissan 240SX - Nissan 240SX (Carbon)
- Nissan 350Z - Nissan 350Z (Carbon)
- Nissan GT-R Proto - Nissan 350Z (Carbon)
- Nissan GT-R R35 - Nissan Skyline GT-R (Carbon)
- Nissan Silvia - Nissan Skyline GT-R (Carbon)
- Nissan Skyline GT-R - Nissan Skyline GT-R (Carbon)
- Pagani Zonda F - Pagani Zonda F (Carbon)
- Plymouth HEMI Cuda - Plymouth HEMI Cuda (Carbon)
- Plymouth Road Runner - Plymouth Road Runner (Carbon)
- Pontiac GTO '65 - Plymouth HEMI Cuda (Carbon)
- Pontiac GTO - Pontiac GTO (Most Wanted)
- Pontiac Solstice GXP - Pontiac GTO (Most Wanted)
- Porsche 911 GT2 - Porsche 911 GT2 (Most Wanted)
- Porsche 911 GT3 - Porsche 911 Carrera S (Most Wanted)
- Porsche 911 GT3 RS - Porsche 911 Carrera S (Most Wanted)
- Porsche 911 Turbo - Porsche 911 Turbo (Most Wanted)
- Porsche Carrera GT - Porsche Carrera GT (Most Wanted)
- Porsche Cayman S - Porsche Cayman S (Most Wanted)
- Seat Leon Cupra - Fiat Punto (Most Wanted)
- Subaru Impreza WRX STI - Subaru Impreza WRX STI (Most Wanted)
- Toyota Corolla - Toyota Corolla (Carbon)
- Toyota Supra - Toyota Supra (Most Wanted)
- Volkswagen Golf GTI - Volkswagen Golf GTI (Most Wanted)
- Volkswagen R32 - Volkswagen R32 (Carbon)
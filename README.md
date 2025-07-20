# Dungeon Slayer

**Dungeon Slayer** is a third-person dungeon action prototype built in Unreal Engine 5.3.2. The player explores a dungeon, fights patrolling enemies, picks up weapons, destroys pots for treasure and health, and uses stamina-based combat mechanics with motion-warped animation and root motion.

> This project is a code showcase only — no build included.

---

## Features

- Player & Enemy State Machines using enums to prevent overlapping actions
- Stamina-based Combat System  
  - Attacks and dodge rolls consume stamina  
  - Health and stamina managed via a custom attribute component
- Enemy AI Patrols with up to 3 enemy types
- Weapon Pickup System  
  - Switch weapons mid-game  
- Breakable Objects  
  - Pots drop treasure or health pickups
- Vaulting System using root motion and motion warping  
  - Supports varying obstacle sizes
- Enhanced Input System for scalable input mapping
- Flexible Tooltip System introduces player to mechanics dynamically

---

## Tech Stack

- Unreal Engine 5.3.2
- C++ (all logic and systems)
- Blueprint (in-game actors derived from C++ base classes)
- Enhanced Input
- Motion Warping
- Custom Attribute Component (Health & Stamina)

---

## Demo

[Gameplay demo](https://youtu.be/ff_GnBkfFFY?si=xIFrBGFx6UZBagJO)

---

## Author

Developed entirely by Ben Luca Jung  
Gameplay programmer with a focus on clean C++ architecture and gameplay systems.

---

## Note

This repository is code-only. Due to asset size, the project does not include game content or binaries. See source code comments for implemetation details.

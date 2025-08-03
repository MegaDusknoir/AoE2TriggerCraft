# Trigger Craft

**this project is deprecated, see [Trigger Craft II](https://github.com/MegaDusknoir/AoE2TriggerCraft2) instead.**

A Scenario GUI Editor in C++, for **Age of Empires 2 Definitive Edition**.

[中文简体](README.zh_cn.md)

## Table of Contents

- [Background](#background)
- [Install](#install)
- [Notes](#notes)
- [Usage](#usage)
    - [What can it do at present](#what-can-it-do-at-present)
- [API](#api)
- [Contributing](#contributing)
- [License](#license)

## Background

We have benefited from [AoKTS](https://github.com/mullikine/aokts) in AoC age and [AoE2ScenarioParser](https://github.com/KSneijders/AoE2ScenarioParser) in DE age for a long time. Even so, we need a faster way for scenario reading, saving, and editing, and a more humanize interface for general scenario author.

This project includes a win32 GUI program and the scenario decoding method written in C++. We hope it can make scenario crafting faster, and having more possibility.

## Install

Extract to a folder, and just double click to run.

## Notes

We will try to make it safe, but **NEVER FORGET** to back up your scenario.

Multi-language is planned when coding, but only **Simplified Chinese** resources is complete.

Localized string will be loaded from internal `string-table` and external file `string.json`.

## Usage

Temporary, to decrease the work to do, we only support the newest scenario format when every last update (1.46 for now).
### What can it do at present

- Read and save scenario.
- Import/Export strings in scenario.
- View trigger tree, show the cond/effe belong to them.
- Duplicate a trigger to every player, modifying the player param, for multiplay.
- View(only) some scenario base info.
- View(only) trigger info.
- Add/Remove triggers/conditions/effects.

## API

Nothing yet.

## Contributing

The GUI project used a lots of code from [AoKTS](https://github.com/mullikine/aokts), thanks to the contributors in 10+ years.

AoE2DE Scenario file format information referenced to [AoE2ScenarioParser](https://github.com/KSneijders/AoE2ScenarioParser) project.

### To contribute
Welcome, or rather look forward, to dive in, [Open an issue](https://github.com/MegaDusknoir/AoE2TriggerCraft/issues/new) or submit PRs。

## License

[GNU General Public License v3.0](LICENSE)

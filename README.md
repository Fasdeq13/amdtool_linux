# amdtool_linux

🌐 **[Русский](#русский) | [English](#english)**

---

## Русский

> **🚨 ВНИМАНИЕ: СТАДИЯ ALPHA 0.1**  
> Данное программное обеспечение находится на **ранней стадии разработки Alpha 0.1**. Половина целевых функций ядра еще не реализована полностью или может работать нестабильно на некоторых конфигурациях системы. Используйте на свой страх и риск.

### Обзор инфраструктуры проекта

`amdtool_linux` — это высокопроизводительная графическая панель управления с открытым исходным кодом, разработанная для видеокарт AMD Radeon в средах Linux. Основная цель проекта — предоставить копию интерфейса официальной утилиты **AMD Software: Adrenalin Edition** (эксклюзивной для Windows) для open-source экосистемы.

Архитектура ядра приложения использует стандарт **C++17** совместно с фреймворком **Qt6 Widgets**, что гарантирует минимальные накладные расходы и отсутствие лишних зависимостей в рантайме.

### Дорожная карта и текущий прогресс
* **Адаптивный интерфейс Adrenalin:** Точная копия оригинального дашборда для отслеживания динамических показателей железа.
* **Автоматическое сканирование игр:** Сканер игровых библиотек без внешних зависимостей, парсящий файлы манифестов (`~/.steam` и `~/.local/share/Steam`).
* **Низкоуровневый мониторинг:** Архитектура запросов в реальном времени, задействующая официальные хуки библиотеки `AMD SMI API`.
* **Графический редактор охлаждения:** Интерактивный блок настройки пользовательской кривой вентиляторов (Smart Fan Curve) с поддержкой прямой записи в железо.
* **Кросс-вендорный режим виртуализации:** Интегрированная система автоматического обнаружения видеокарт NVIDIA для безопасной отладки интерфейса на хост-системах без GPU AMD.

---

### Документация и спецификации разработки

Для детального разбора архитектуры, инструкций по развертыванию на специфических слоях ядра, структур API и чертежей компиляции перейдите к основной базе знаний проекта:

👉 **[Перейти к официальной Wiki проекта и руководству по функциям](https://github.com/Fasdeq13/amdtool_linux/wiki)**

---

### Технические требования к сборке

#### Зависимости для компиляции
* `cmake` (>= 3.16)
* `g++` (>= 11.0)
* `qt6-base-dev`

#### Процесс компиляции
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j\$(nproc)
```

#### Генерация пакетов дистрибутивов
```bash
cpack -G DEB   # Сборка пакета для Debian/Ubuntu
cpack -G RPM   # Сборка пакета для Fedora/RedHat
```

<br>
<hr>
<br>

## English

> **🚨 WARNING: ALPHA 0.1 STAGE**  
> This software is currently in the **early Alpha 0.1 development stage**. Half of the target core infrastructure features are not fully implemented yet or might not function as expected on certain configurations. Use at your own risk.

### Project Infrastructure Overview

`amdtool_linux` is an open-source, high-performance graphical control panel suite designed natively for AMD Radeon hardware running under Linux desktop environments. The primary goal of this project, feature-rich interface clone of the official Windows-exclusive **AMD Software: Adrenalin Edition**, tailored entirely for the open-source ecosystem.

The application core architecture utilizes **C++17** alongside the **Qt6 Widgets** engine framework to guarantee minimal runtime overhead with zero external garbage dependencies.

### Features Roadmap & Progress
* **Adaptive Dashboard Overlay:** A replica of the official Adrenalin layout tracking dynamic hardware stats.
* **Automated Game Engine Tracking:** Zero-runtime dependency scanner parsing local filesystem appmanifest files (`~/.steam` & `~/.local/share/Steam`) to map game libraries.
* **Low-Level Metric Tracing:** Real-time query infrastructure bridging official `AMD SMI API` hooks.
* **Interactive Tuning Pipeline:** Dynamic custom Smart Fan Curve graphical editor block supporting raw hardware overrides.
* **Cross-Vendor Redirection System:** Integrated automated NVIDIA hardware detection path converting rendering states into virtual mock structures for safe host environment debugging.

---

### Documentation & Development Specs

For architectural breakdowns, precise deployment instructions across specific kernel layers, API structures, and compilation blueprints, proceed to the primary project knowledge base:

👉 **[View the Official Project Wiki & Feature Manual](https://github.com/Fasdeq13/amdtool_linux/wiki)**

---

### Technical Building Requirements

#### Build Dependencies
* `cmake` (>= 3.16)
* `g++` (>= 11.0)
* `qt6-base-dev`

#### Compilation Pipeline
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j\$(nproc)
```

#### Package Generation Engine
```bash
cpack -G DEB   # Compiles native Debian/Ubuntu deployment layer
cpack -G RPM   # Compiles native Fedora/RedHat deployment layer
```

---
*Developed by [Fasdeq13](https://github.com/Fasdeq13). Distributed under the terms of the MIT License.*

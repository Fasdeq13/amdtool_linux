# amdtool_linux

🌐 **Русский | English**

---

## Русский

**🚀 СТАБИЛЬНЫЙ РЕЛИЗ v0.1.0**

Проект успешно перенесен на высокопроизводительную кодовую базу **Python 3 & PySide6 (Qt6 Widgets)**, полностью отлажен и готов к использованию. Все зависимости теперь изолированы, а сборка упакована в нативные пакеты для бесшовной интеграции в операционную систему.

### Обзор инфраструктуры проекта

amdtool_linux — это продвинутая графическая панель управления с открытым исходным кодом, разработанная для видеокарт AMD Radeon в средах Linux. Основная цель проекта — предоставить точную, отзывчивую и легкую копию интерфейса официальной утилиты **AMD Software: Adrenalin Edition** для open-source экосистемы, избавив пользователей от необходимости копаться в конфигурационных файлах и терминале.

### Реализованные функции (Релиз v0.1.0)

*   **Адаптивный дашборд Adrenalin:** Выверенная темная тема оригинального приложения с мгновенным откликом и плавной навигацией по вкладкам.
*   **Умный сканер игровых библиотек (Gaming):** Автоматический поиск установленных игр в Steam (включая проверку путей нативных систем). Полностью исключает системный мусор, Proton-рантаймы и служебные инструменты Steamworks Shared.
*   **Интегрированные параметры запуска:** Автоматическая генерация и копирование строк оптимизации Vulkan/Mesa для каждой игры (включая тонкие флаги `-nojoy -novid` для нативных движков Valve).
*   **Песочница и Контейнеризация:** Умное обнаружение изолированных игровых сред. Система автоматически определяет Flatpak-версии игр и выводит уведомление о технических ограничениях мониторинга.
*   **Интерактивный эмулятор апгрейда ядра (Driver):** Встроенный терминал обновления системы, поддерживающий кросс-дистрибутивную автоматизацию команд для **30+ пакетных менеджеров** (от `apt` и `pacman` до `nix`, `guix` и `kiss`). Запросы прав администратора изолированы через безопасное графическое окно `pkexec`.
*   **Мониторинг графических API:** Потоковый вывод текущих версий Vulkan Core API и OpenGL Mesa профилей системы в реальном времени.
*   **Performance AMD Guard:** Интеллектуальный контроль графического слоя, блокирующий несовместимое стороннее железо (Intel/Nvidia) и проверяющий статус низкоуровневого демона мониторинга частот **LACT**.

### База знаний и Документация

Для детального разбора архитектуры проекта, инструкций по развертыванию, интеграции сторонних модулей и руководства по тонкой настройке перейдите в официальный раздел документации:

👉 **[Перейти к официальной Wiki проекта и руководству по функциям](https://github.com/Fasdeq13/amdtool_linux/wiki)**

### Установка готовых пакетов

Вы можете скачать готовые скомпилированные пакеты со всеми встроенными иконками и дескриптор-файлами в меню релизов на этой странице.

*   **Для Debian / Ubuntu / Mint / Pop!_OS:** `sudo apt install ./amdtool_1.0.0_amd64.deb`
*   **Для Fedora / RHEL Workstation / openSUSE:** `sudo dnf install ./amdtool-1.0.0-1.x86_64.rpm`

---

## English

**🚀 STABLE RELEASE v0.1.0**

The core ecosystem has been successfully migrated to a high-performance, rock-solid **Python 3 & PySide6 (Qt6 Widgets)** codebase. The application layers are fully optimized, runtime tracking is isolated, and native pipeline deployments are packaged for cross-distribution compatibility.

### Project Infrastructure Overview

amdtool_linux is an open-source, lightweight graphical control panel suite built natively for AMD Radeon hardware running under Linux desktop environments. The primary milestone of this project is providing a feature-rich, pixel-perfect interface clone of the official Windows-exclusive **AMD Software: Adrenalin Edition**, tailored entirely to eliminate terminal routine for open-source users.

### Implemented Core Features (Release v0.1.0)

*   **Adaptive Adrenalin Dashboard:** A high-fidelity replica of the official dark Adrenalin user interface layout featuring seamless tab switches and zero latency rendering.
*   **Automated Game Library Scanner (Gaming):** Built-in query system searching local Steam assets. It filters out system runtimes, Steamworks Shared data, and Proton tools, leaving only clean game profiles.
*   **One-Click Launch Arguments Engine:** Auto-generates and copies native Vulkan/Mesa optimization launch options per game profile (including performance tweaks like `-nojoy -novid` for native Valve products).
*   **Isolation Sandbox Detection:** Advanced environment handling. The engine tracks Flatpak game instances on the fly, outputting specific technical constraints alerts regarding frame timing limitations.
*   **Embedded Distribution Upgrade Terminal (Driver):** An interactive update sandbox automation script handling native instructions for **30+ Linux package managers** (ranging from `apt` and `pacman` to `nix`, `guix`, and `kiss`). Root authority allocation is securely hooked via native `pkexec` dialog windows.
*   **Pipeline Graphics API Polling:** Real-time summary streams capturing current system-wide Vulkan Core API versions and Mesa OpenGL profile configurations.
*   **Performance AMD Guard:** Intelligent hardware architecture validation path that prevents execution on incompatible non-AMD engines (Intel/Nvidia) and tracks low-level core **LACT** daemon integration states.

### Documentation & Wiki Reference

For structural deep-dives, manual customization parameters, modular injection blueprints, and troubleshooting manuals, proceed to our main knowledge base:

👉 **[Proceed to the Official Project Wiki & Feature Manual](https://github.com/Fasdeq13/amdtool_linux/wiki)**

### Native Deployment Installation

Pre-compiled production-ready standalone packages with bundled desktop assets and application icons are available in the Releases section.

*   **For Debian / Ubuntu / Mint / Pop!_OS:** `sudo apt install ./amdtool_1.0.0_amd64.deb`
*   **For Fedora / RHEL Workstation / openSUSE:** `sudo dnf install ./amdtool-1.0.0-1.x86_64.rpm`

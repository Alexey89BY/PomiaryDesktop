#ifndef SETTINGS_H
#define SETTINGS_H


class Settings
{
public:
    Settings() = delete;

    static void load();
    static void save();
};

#endif // SETTINGS_H

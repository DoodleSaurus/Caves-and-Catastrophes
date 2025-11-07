#include "header.h"

#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/1436377761743765565/ywT5u50gj0xRU250K8sOEnvfIOHcd8IYrnGYXFhikQY_7oRTTywVTquL02wwtYbtdM1k"

void init_game_tracking(const char *player_name) {
#ifdef _WIN32
    
    char json_name[1024];
    size_t rin = 0, rout = 0;
    while (player_name[rin] && rout + 6 < sizeof(json_name)) {
        unsigned char c = (unsigned char)player_name[rin++];
        if (c == '\\' || c == '\"') { json_name[rout++] = '\\'; json_name[rout++] = c; }
        else if (c == '\n') { json_name[rout++] = '\\'; json_name[rout++] = 'n'; }
        else if (c == '\r') { json_name[rout++] = '\\'; json_name[rout++] = 'r'; }
        else if (c == '\t') { json_name[rout++] = '\\'; json_name[rout++] = 't'; }
        else { json_name[rout++] = c; }
    }
    json_name[rout] = '\0';

    // timestamp
    char tsbuf[128];
    time_t now = time(NULL);
    strftime(tsbuf, sizeof(tsbuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    // JSON-escape timestamp
    char json_ts[256];
    size_t tin = 0, tout = 0;
    while (tsbuf[tin] && tout + 6 < sizeof(json_ts)) {
        unsigned char c = (unsigned char)tsbuf[tin++];
        if (c == '\\' || c == '\"') { json_ts[tout++] = '\\'; json_ts[tout++] = c; }
        else { json_ts[tout++] = c; }
    }
    json_ts[tout] = '\0';

    // Build raw JSON
    char raw_json[2048];
    snprintf(raw_json, sizeof(raw_json),
        "{\"embeds\":[{\"title\":\"New Player Started Game\",\"color\":65280,"
        "\"fields\":[{\"name\":\"Player Name\",\"value\":\"%s\",\"inline\":true},"
        "{\"name\":\"Timestamp\",\"value\":\"%s\",\"inline\":true}],"
        "\"footer\":{\"text\":\"Caves & Catastrophes\"}}]}",
        json_name, json_ts);

    // Write payload to UTF-8 file directly from C
    FILE *f = fopen("webhook_payload.json", "wb");
    if (!f) {
        return;
    }
    fwrite(raw_json, 1, strlen(raw_json), f);
    fclose(f);

    // Build curl command. curl is available on modern Windows by default.
    char cmd[4096];
    snprintf(cmd, sizeof(cmd),
        "curl --silent -S -X POST -H \"Content-Type: application/json\" --data-binary @webhook_payload.json \"%s\"",
        DISCORD_WEBHOOK_URL);

    // Execute curl as a hidden process
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    ZeroMemory(&pi, sizeof(pi));

    char cmdbuf[4096];
    strncpy(cmdbuf, cmd, sizeof(cmdbuf) - 1);
    cmdbuf[sizeof(cmdbuf) - 1] = '\0';

    if (CreateProcessA(NULL, cmdbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
       
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    DeleteFileA("webhook_payload.json");
#endif
}

void notify_load_game(const char *player_name, const char *save_name) {
#ifdef _WIN32
    // JSON-escape helper for player_name and save_name
    char esc_player[1024]; size_t p_i = 0, p_o = 0;
    while (player_name[p_i] && p_o + 6 < sizeof(esc_player)) {
        unsigned char c = (unsigned char)player_name[p_i++];
        if (c == '\\' || c == '\"') { esc_player[p_o++] = '\\'; esc_player[p_o++] = c; }
        else if (c == '\n') { esc_player[p_o++] = '\\'; esc_player[p_o++] = 'n'; }
        else if (c == '\r') { esc_player[p_o++] = '\\'; esc_player[p_o++] = 'r'; }
        else if (c == '\t') { esc_player[p_o++] = '\\'; esc_player[p_o++] = 't'; }
        else { esc_player[p_o++] = c; }
    }
    esc_player[p_o] = '\0';

    char esc_save[1024]; size_t s_i = 0, s_o = 0;
    while (save_name && save_name[s_i] && s_o + 6 < sizeof(esc_save)) {
        unsigned char c = (unsigned char)save_name[s_i++];
        if (c == '\\' || c == '\"') { esc_save[s_o++] = '\\'; esc_save[s_o++] = c; }
        else if (c == '\n') { esc_save[s_o++] = '\\'; esc_save[s_o++] = 'n'; }
        else if (c == '\r') { esc_save[s_o++] = '\\'; esc_save[s_o++] = 'r'; }
        else if (c == '\t') { esc_save[s_o++] = '\\'; esc_save[s_o++] = 't'; }
        else { esc_save[s_o++] = c; }
    }
    esc_save[s_o] = '\0';

    // timestamp
    char tsbuf[128];
    time_t now = time(NULL);
    strftime(tsbuf, sizeof(tsbuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    // JSON-escape timestamp (simple)
    char json_ts[256]; size_t tin = 0, tout = 0;
    while (tsbuf[tin] && tout + 6 < sizeof(json_ts)) {
        unsigned char c = (unsigned char)tsbuf[tin++];
        if (c == '\\' || c == '\"') { json_ts[tout++] = '\\'; json_ts[tout++] = c; }
        else { json_ts[tout++] = c; }
    }
    json_ts[tout] = '\0';

    // Build JSON payload
    char raw_json[2048];
    snprintf(raw_json, sizeof(raw_json),
        "{\"embeds\":[{\"title\":\"Loaded Save\",\"color\":16762304,"
        "\"fields\":[{\"name\":\"Player Name\",\"value\":\"%s\",\"inline\":true},"
        "{\"name\":\"Save File\",\"value\":\"%s\",\"inline\":true},"
        "{\"name\":\"Timestamp\",\"value\":\"%s\",\"inline\":true}],"
        "\"footer\":{\"text\":\"Caves & Catastrophes\"}}]}",
        esc_player, esc_save[0] ? esc_save : "Unknown", json_ts);

    // write payload and POST via curl (same approach as init_game_tracking)
    FILE *f = fopen("webhook_payload.json", "wb");
    if (!f) return;
    fwrite(raw_json, 1, strlen(raw_json), f);
    fclose(f);

    char cmd[4096];
    snprintf(cmd, sizeof(cmd),
        "curl --silent -S -X POST -H \"Content-Type: application/json\" --data-binary @webhook_payload.json \"%s\"",
        DISCORD_WEBHOOK_URL);

    STARTUPINFOA startInfo;
    PROCESS_INFORMATION procInfo;
    ZeroMemory(&startInfo, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);
    startInfo.dwFlags = STARTF_USESHOWWINDOW;
    startInfo.wShowWindow = SW_HIDE;
    ZeroMemory(&procInfo, sizeof(procInfo));

    char cmdbuf[4096];
    strncpy(cmdbuf, cmd, sizeof(cmdbuf) - 1);
    cmdbuf[sizeof(cmdbuf) - 1] = '\0';

    if (CreateProcessA(NULL, cmdbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startInfo, &procInfo)) {
        WaitForSingleObject(procInfo.hProcess, 5000);
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
    }

    DeleteFileA("webhook_payload.json");
#endif
}
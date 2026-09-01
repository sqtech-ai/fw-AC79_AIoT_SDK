//
//  IOTSdkBridge.h
//
#ifndef _IOT_SDK_BRIDGE_H_
#define _IOT_SDK_BRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif

int IOTSdk_Init(const char* workspace, const char* args);
/**
 * @param output 必须调用free()释放
 */
int IOTSdk_Search(const char* input, char** output, int timeout);
/**
 * @param output 必须调用free()释放
 */
int IOTSdk_Report(const char* input, char** output, int timeout);
/**
 * @param output 必须调用free()释放
 */
int IOTSdk_GetMusicInfo(const char* input, char** output, int timeout);
/**
 * @param output 必须调用free()释放
 */
int IOTSdk_QueryMusicSheet(const char* input, char** output, int timeout);
/**
 * @param output 必须调用free()释放
 */
int IOTSdk_QueryMusicSheetMusic(const char* input, char** output, int timeout);

#ifdef __cplusplus
}
#endif

#endif//_IOT_SDK_BRIDGE_H_

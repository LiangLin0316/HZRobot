//
// Created by Lin on 2020/8/20.
//

#ifndef __LIN_HTTP_MARK_H__
#define __LIN_HTTP_MARK_H__


#define HTTP_SUCCESS       (0)          // 操作成功
#define HTTP_ERROR_UNKNOWN      (-1)         // 未知的错误
#define HTTP_ERROR_NETWORK      (-2)         // 网络连接失败
#define HTTP_ERROR_HTTP_HEAD    (-3)         // 未找到协议头 http || https 

#define HTTP_ERROR_SERVICE      (-1000)      // 服务器请求失败
#define HTTP_ERROR_LOGIN        (-1001)      // 登录失败
#define HTTP_ERROR_ID           (-1002)      // 企业ID错误
#define HTTP_ERROR_USER         (-1003)      // 帐号不存在
#define HTTP_ERROR_PASSWORD     (-1004)      // 密码错误

#define HTTP_ERROR_PARAMETER    (1)          // 参数错误
#define HTTP_ERROR_PHONE        (2)          // 电话号码错误
#define HTTP_ERROR_MESSAGE      (3)          // 短信有屏蔽字段
#define HTTP_ERROR_FUNCTION     (4)          // 当前平台不支持这个功能


#endif // __LIN_HTTP_MARK_H__
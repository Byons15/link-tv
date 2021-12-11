using System;
using System.Text;

namespace LinkServer.Components
{
    public static class Utils
    {
        private static readonly char[] charTable =
                    {
                    '0','1','2','3','4','5','6','7','8','9',
                    'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
                    };

        private static readonly Random random = new();

        public static string CreateRandomStr(int length)
        {
            StringBuilder builder = new(length);

            for (var i = 0; i < length; ++i)
            {
                builder.Append(charTable[random.Next(charTable.Length)]);
            }

            return builder.ToString();
        }

        /// <summary>
        /// 用户名正则表达式，必须以英文字母或中文开头，只允许使用中文、英文字母、数字、下划线，长度在6-64之间
        /// </summary>
        public const string UserNameRegex = @"^([a-zA-Z\u4e00-\u9fa5][a-zA-Z0-9_\u4e00-\u9fa5]{6,64})$";

        /// <summary>
        /// 用户密码正则表达式，由数字和字母组成，并且要同时含有数字和字母，且长度要在8-16位之间。
        /// </summary>
        public const string UserPasswordRegex = @"^(?![0-9]+$)(?![a-zA-Z]+$)[0-9A-Za-z_]{8,128}$";
    }
}

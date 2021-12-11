using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Linkme.Common;
using Linkme.Commands;
using System.Windows;

namespace Linkme.Login
{
    class Login : NotificationObject
    {
        public LoginWindow Window;

        public Login()
        {
            Window = new LoginWindow
            {
                DataContext = this
            };
        }

        private LoginModel obj = new LoginModel();

        public string User
        {
            get
            {
                return obj.User;
            }

            set
            {
                obj.User = value;
                RaisePropertyChanged("User");
            }
        }

    }
}

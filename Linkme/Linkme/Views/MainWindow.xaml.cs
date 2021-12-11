using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace Linkme.Views
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private DispatcherTimer paintTimer;

        public MainWindow()
        {
            InitializeComponent();

            //paintTimer = new DispatcherTimer(DispatcherPriority.ApplicationIdle);
            //paintTimer.Tick += (sender, e) =>
            //{
            //    canvasView_Paint(null, null);
            //};
            //paintTimer.Start();
        }

        //private void canvasView_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        //{
        //    var vm = DataContext as ViewModels.Linkme;
        //    vm.CanvasViewPaint(canvasView);
        //}
    }
}

using System;
using LinkmeLib;
using Prism.Commands;
using System.Windows;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;

namespace Linkme.ViewModels
{
    class Linkme : DependencyObject, INotifyPropertyChanged, IDisposable
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public DelegateCommand<Control> CanvasViewLoadedCommand { get; set; }

        public DelegateCommand<SizeChangedEventArgs> CanvasViewResizeCommand { get; set; }

        public DelegateCommand<object> BeginRecordCommand { get; set; }

        public DelegateCommand<object> BeginLiveCommand { get; set; }

        private bool recording = false;
        public bool Recording
        {
            get => recording;
            set
            {
                recording = value;
                RaisePropertyChanged(nameof(Recording));
            }
        }

        public DelegateCommand<object> StopReocrdCommand { get; set; }

        private Canvas canvas;

        private AudioCapturer speakerCapturer;

        private AudioCapturer microphoneCapturer;

        private Encoder encoder;

        private DesktopCapturer deskCapturer = new DesktopCapturer(0);

        private void MakeCommands()
        {
            CanvasViewLoadedCommand = new DelegateCommand<Control>(new Action<Control>(control =>
            {
                canvas = new Canvas(1920, 1080, control.Handle);
                canvas.AddSprite(deskCapturer);
            }));

            CanvasViewResizeCommand = new DelegateCommand<SizeChangedEventArgs>(new Action<SizeChangedEventArgs>(e =>
            {
                canvas.ViewResize((int)e.NewSize.Width, (int)e.NewSize.Height);
            }));

            BeginRecordCommand = new DelegateCommand<object>(new Action<object>(obj =>
            {
                StartLive(@"E:/Video/test.mp4");
            }));

            BeginLiveCommand = new DelegateCommand<object>(new Action<object>((obj) => 
            {
                StartLive(@"rtmp://172.30.51.197:1935/live/331?username=byons&password=eyJhbGciOiJodHRwOi8vd3d3LnczLm9yZy8yMDAxLzA0L3htbGRzaWctbW9yZSNobWFjLXNoYTI1NiIsInR5cCI6IkpXVCJ9.eyJodHRwOi8vc2NoZW1hcy54bWxzb2FwLm9yZy93cy8yMDA1LzA1L2lkZW50aXR5L2NsYWltcy9uYW1lIjoiMTAwMDAiLCJpZCI6IjEwMDAwIiwibmFtZSI6ImJ5b25zIiwibmJmIjoxNjMwNTkwNjkxLCJleHAiOjE2MzA2NzcwOTEsImlzcyI6IkxpbmtMaXZlIiwiYXVkIjoiTGlua0xpdmUifQ.K_lxSnbza20zibVR8XDZujsrajNhwTNcYoV8qPSHBrE");
            }));

            StopReocrdCommand = new DelegateCommand<object>(new Action<object>(obj => 
            {
                StopRecord();
            }));
        }

        private Timer liveTimer;

        private TimeSpan liveClock;

        public TimeSpan LiveClock
        {
            get => liveClock;
            set
            {
                liveClock = value;
                RaisePropertyChanged(nameof(LiveClock));
            }
        }


        public Linkme()
        {
            MakeCommands();
        }

        public void StartLive(string outputUrl) 
        {
            speakerCapturer = new AudioCapturer(AudioDevice.Speaker);

            AudioInfo[] audioInputs = new AudioInfo[1];
            audioInputs[0] = speakerCapturer.Format;

            encoder = new Encoder(1920, 1080, 60, 5000000, audioInputs, outputUrl);

            canvas.StartOutput(1920, 1080, 60, encoder);
            speakerCapturer.Start(encoder, 0);

            liveClock = new TimeSpan(0);

            liveTimer = new Timer
            {
                Interval = 1000
            };
            liveTimer.Tick += (sender, e) =>
            {
                LiveClock = LiveClock.Add(new TimeSpan(0, 0, 1));
            };
            liveTimer.Start();

            Recording = true;
        }

        public void StopRecord()
        {
            canvas.StopOutput();
            speakerCapturer.Stop();

            speakerCapturer.Dispose();
            speakerCapturer = null;

            encoder.Dispose();
            encoder = null;

            Recording = false;
            liveTimer.Stop();
            LiveClock = new TimeSpan(0);
        }

        internal void CanvasViewPaint(Control control)
        {
            if (canvas is null)
                return;

            canvas.ViewPaint();
            control.Invalidate();
        }

        public void Dispose()
        {
            canvas.RemoveSprite(deskCapturer);
        }
    }
}

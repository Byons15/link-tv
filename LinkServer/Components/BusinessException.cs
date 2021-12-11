using System;

namespace LinkServer.Components
{

    [Serializable]
    public class BusinessException : Exception
    {
        public BusinessException(Exception inner = null) : base(null, inner) { }

        public BusinessException(string message, int stateCode) : base(message) 
        { 
            StatusCode = stateCode;  
        }

        public BusinessException(string message, int stateCode, Exception inner) : base(message, inner) 
        {
            StatusCode = stateCode;
        }

        protected BusinessException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context) : base(info, context) { }

        public virtual int StatusCode { get; }

        public virtual string MessageType { get; } = "text/plain; charset=utf-8";
    }
}

using System;

namespace LinkServer.Components
{
    public class InternalException : BusinessException
    {
        public InternalException(string message) : base(message, 500) { }
        public InternalException(string message, Exception inner) : base(message, 500, inner) { }
        protected InternalException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context) : base(info, context) { }
    }
}

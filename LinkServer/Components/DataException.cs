using System;

namespace LinkServer.Components
{
    [Serializable]
    public class DataException : BusinessException
    {
        public DataException(string message) : base(message, 500) { }
        public DataException(string message, Exception inner) : base(message, 500, inner) { }
        protected DataException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context) : base(info, context) { }
    }
}

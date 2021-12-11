using LinkServer.Models;
using Newtonsoft.Json.Schema;
using NSwag.Generation.Processors;
using NSwag.Generation.Processors.Contexts;
using System.IO;

namespace LinkServer.Components
{
    public class ResponseProcessor : IDocumentProcessor
    {
        public void Process(DocumentProcessorContext context)
        {
            AddResponse(context, typeof(InvalidModelDescription), "400", "请求的参数无效时返回此模型。");
            AddResponse(context, typeof(InternalErrorDescription), "500", "服务器内部发生错误时返回此模型。");
        }

        private void AddResponse(DocumentProcessorContext context, System.Type responseModelType, string statusCode, string description)
        {
            var sourceResponse = new NSwag.OpenApiResponse()
            {
                Schema = context.SchemaGenerator.Generate(responseModelType),
                Description = description
            };

            context.Document.Responses[responseModelType.Name] = sourceResponse;

            foreach(var operation in context.Document.Operations)
            {
                operation.Operation.Responses[statusCode] = new()
                {
                    Reference = sourceResponse
                };
            }
        }
    }
}

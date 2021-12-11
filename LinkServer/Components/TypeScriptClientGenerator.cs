
using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.Configuration;
using NSwag;
using NSwag.CodeGeneration.TypeScript;
using System.Threading.Tasks;
using System.IO;

namespace LinkServer.Components
{
    static public class TypeScriptClientGenerator
    {
        public static async Task<IApplicationBuilder> UseTypeScriptClientGeneratorAsync(this IApplicationBuilder applicationBuilder, string jsonUrl)
        {
            var document = await OpenApiDocument.FromUrlAsync(jsonUrl);

            var settings = new TypeScriptClientGeneratorSettings
            {
                ClassName = "{controller}Client",
            };
            settings.Template = TypeScriptTemplate.Axios;
            settings.PromiseType = PromiseType.Promise;

            var generator = new NSwag.CodeGeneration.TypeScript.TypeScriptClientGenerator(document, settings);
            var code = generator.GenerateFile();

            
            using(var clientSrc = File.CreateText("../linktv/src/LinkClient.ts"))
            {
                clientSrc.Write(code);
            }

            return applicationBuilder;
        }
    }
}

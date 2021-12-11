using Microsoft.IdentityModel.Tokens;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IdentityModel.Tokens.Jwt;
using System.Linq;
using System.Security.Claims;
using System.Text;
using LinkServer.Components;
using Microsoft.Extensions.Configuration;

namespace LinkServer.Services
{
    public class TokenService
    {
        class Options
        {
            public string SecurityKey { get; set; }
            public string Issuer { get; set; }
            public string Audience { get; set; }
        }

        private readonly Options options = new();

        public TokenService(IConfiguration configuration)
        {
            configuration.GetSection("TokenServiceOptions").Bind(options);
        }

        public string SecurityKey { get => options.SecurityKey; }

        static private IEnumerable<Claim> GetUserClaims(long id)
        {
            IEnumerable<Claim> claims = new Claim[]
            {
                new Claim("id", id.ToString())
            };
            return claims;
        }

        public string CreateToken(long id)
        {
            var JWToken = new JwtSecurityToken(
            issuer: options.Issuer,
            audience: options.Audience,
            notBefore: DateTime.Now,
            expires: DateTime.Now.AddDays(1),
            claims: GetUserClaims(id),
            signingCredentials: new SigningCredentials(new SymmetricSecurityKey(Encoding.ASCII.GetBytes(options.SecurityKey)),
                                SecurityAlgorithms.HmacSha256Signature)
            );

            return new JwtSecurityTokenHandler().WriteToken(JWToken);
        }

        public long Resolve(string token)
        {
            JwtSecurityTokenHandler tokenHandler = new();
            TokenValidationParameters validationParameters = new()
            {
                ValidateIssuerSigningKey = true,
                IssuerSigningKey = new SymmetricSecurityKey(Encoding.ASCII.GetBytes(options.SecurityKey)),
                ValidateIssuer = true,
                ValidIssuer = options.Issuer,
                ValidateAudience = true,
                ValidAudience = options.Audience,
                RequireExpirationTime = true,
                ValidateLifetime = true,
                ClockSkew = TimeSpan.Zero
            };
            try
            {
                var claims = tokenHandler.ValidateToken(token, validationParameters, out SecurityToken securityToken);
                return long.Parse(claims.Claims.First(claim => claim.Type == "id").Value);
            }
            catch (Exception ex)
            {
                throw new InvalidModelException("令牌解析失败", ex);
            }
        }
    }


}
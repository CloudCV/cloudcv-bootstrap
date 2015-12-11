
function capitalizeFirstLetter(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function describeAlgorithm(algorithmName) {

    return {
        get: {
            summary: "Returns information about algorithm",
            description: "",
            operationId: "get" + capitalizeFirstLetter(algorithmName) + "Info",
            consumes: ["application/json"],
            produces: ["application/json"],
        },
        post: {
            summary: "Invokes method",
            description: "",
            operationId: algorithmName,
            consumes: ["application/json"],
            produces: ["application/json"],
            parameters: [
                {
                    in: "body",
                    name: "body",
                    description: "Input arguments",
                    required: true,
                    schema: {
                        "$ref": "#/definitions/" + capitalizeFirstLetter(algorithmName) + "Input"
                    }
                }
            ],
            responses: {
                200: {
                    description: "successful operation",
                    schema: {
                        "$ref": "#/definitions/" + capitalizeFirstLetter(algorithmName) + "Result"
                    }
                },
                400: {
                    description: "Invalid status value"
                }
            },
        },
    };
}

module.exports.getSpec = function (algorithms) {
    var i;
    var spec = {
        swagger: "2.0",
        info: {
            description: "This is a sample service of CloudCV server.",
            version: "1.0.0",
            title: "CloudCV",
            termsOfService: "http://cloudcv.io/terms/",
            contact: {
                email: "ekhvedchenya@gmail.com"
            },
            license: {
                name: "Apache 2.0",
                url: "http://www.apache.org/licenses/LICENSE-2.0.html"
            },
            basePath: "/v2",
            tags: [
                {
                    name: "pet",
                    description: "Everything about your Pets",
                    externalDocs: {
                        description: "Find out more",
                        url: "http://swagger.io"
                    }
                },
                {
                    name: "store",
                    description: "Access to Petstore orders"
                },
                {
                    name: "user",
                    description: "Operations about user",
                    externalDocs: {
                        description: "Find out more about our store",
                        url: "http://swagger.io"
                    }
                }
            ],
            schemes: [ "http", "https" ],
            securityDefinitions: {
                /*
                petstore_auth: {
                    "type": "oauth2",
                    "authorizationUrl": "http://petstore.swagger.io/api/oauth/dialog",
                    "flow": "implicit",
                    "scopes": {
                        "write:pets": "modify pets in your account",
                        "read:pets": "read your pets"
                    }
                },
                api_key: {
                    "type": "apiKey",
                    name: "api_key",
                    "in": "header"
                }
                */
            },
        },
        paths: {},
        definitions: {},
        externalDocs: {
            description: "Find out more about Swagger",
            url: "http://swagger.io"
        },
        host: "petstore.swagger.io",
    };

    for (i = algorithms.length - 1; i >= 0; i--) {
        spec.paths[algorithms[i]] = describeAlgorithm(algorithms[i]);
    }

    return spec;
};
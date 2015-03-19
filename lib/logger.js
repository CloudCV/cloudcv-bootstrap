/**********************************************************************************
 * CloudCV Boostrap - A starter template for Node.js with OpenCV bindings.
 *                    This project lets you to quickly prototype a REST API
 *                    in a Node.js for a image processing service written in C++. 
 * 
 * Author: Eugene Khvedchenya <ekhvedchenya@gmail.com>
 * 
 * More information:
 *  - https://cloudcv.io
 *  - http://computer-vision-talks.com
 * 
 **********************************************************************************/

 var path = require('path')
  , fs = require('fs')
  , winston = require('winston')
  ;


var logsDir = process.env.LOGS || './logs'

var logger = new (winston.Logger)({
    transports: [
        new (winston.transports.Console)(),
        new (winston.transports.File)({
            name: 'info-file',
            filename: logsDir + '/api.cloudcv.io.info.log',
            level: 'info'
        }),
        new (winston.transports.File)({
          name: 'error-file',
          filename: logsDir + '/api.cloudcv.io.error.log',
          level: 'error'
        })
    ]
});

if (!fs.existsSync(logsDir)) {    
    fs.mkdirSync(logsDir);

    logger.info('Created logs directory %s', logsDir);
}


module.exports = logger;
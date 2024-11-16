const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');
const IotData = new AWS.IotData({ endpoint: 'a2xkr2m0uy0ejv-ats.iot.us-east-2.amazonaws.com' });

const EnableEmergencyParams = {
    thingName: 'ESP32',
    payload: '{"state": {"desired": {"alarmEnabled": true}}}',
};

const DisableEmergencyParams = {
    thingName: 'ESP32',
    payload: '{"state": {"desired": {"alarmEnabled": false}}}',
};

const ShadowParams = {
    thingName: 'ESP32',
};

function getShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.getThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject(`Error al obtener el estado del invernadero: ${err.errorMessage}`);
            } else {
                resolve(JSON.parse(data.payload));
            }
        });
    });
}

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    handle(handlerInput) {
        const speakOutput = 'Bienvenido al sistema de invernadero inteligente. Puedes activar o desactivar la alarma de emergencia o consultar el estado actual. ¿Qué deseas hacer?';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const TurnOnEmergencyIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'TurnOnEmergencyIntent';
    },
    handle(handlerInput) {
        const params = EnableEmergencyParams;

        IotData.updateThingShadow(params, function (err, data) {
            if (err) {
                console.log('Error al activar la alarma de emergencia:', err);
            } else {
                console.log('Alarma de emergencia activada:', data);
            }
        });

        const speakOutput = 'EMERGENCIA: La alarma del invernadero ha sido activada. Verifica el estado del sistema inmediatamente.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const TurnOffEmergencyIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'TurnOffEmergencyIntent';
    },
    handle(handlerInput) {
        const params = DisableEmergencyParams;

        IotData.updateThingShadow(params, function (err, data) {
            if (err) {
                console.log('Error al desactivar la alarma de emergencia:', err);
            } else {
                console.log('Alarma de emergencia desactivada:', data);
            }
        });

        const speakOutput = 'La alarma del invernadero ha sido desactivada. Todo está bajo control.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CheckEmergencyStatusIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'CheckEmergencyStatusIntent';
    },
    async handle(handlerInput) {
        let alarmEnabled = 'unknown';

        await getShadowPromise(ShadowParams).then((result) => {
            if (result.state && result.state.reported && result.state.reported.alarmEnabled !== undefined) {
                alarmEnabled = result.state.reported.alarmEnabled;
            }
        }).catch((err) => {
            console.log('Error al consultar el estado del shadow:', err);
        });

        let speakOutput = 'Error al consultar el estado del sistema.';
        if (alarmEnabled === true) {
            speakOutput = 'EMERGENCIA: La alarma del invernadero está activada. Verifica el sistema inmediatamente.';
        } else if (alarmEnabled === false) {
            speakOutput = 'La alarma del invernadero está desactivada. Todo está funcionando normalmente.';
        }

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Puedes activar o desactivar la alarma de emergencia o consultar el estado actual en tu invernadero inteligente. ¿Cómo puedo ayudarte?';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Hasta luego.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};

const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Lo siento, no entendí esa solicitud. Por favor, inténtalo de nuevo.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Sesión terminada: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        return handlerInput.responseBuilder.getResponse();
    }
};

const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Disculpa, ocurrió un error. Por favor intenta de nuevo.';
        console.log(`~~~~ Error manejado: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        TurnOnEmergencyIntentHandler,
        TurnOffEmergencyIntentHandler,
        CheckEmergencyStatusIntentHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler)
    .addErrorHandlers(ErrorHandler)
    .withCustomUserAgent('sample/greenhouse-system/v1.0')
    .lambda();

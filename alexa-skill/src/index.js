'use strict';

var Alexa = require('alexa-sdk');

// Initialize fireBase with a service account, granting admin privileges
var firebaseAdmin = require("firebase-admin");
firebaseAdmin.initializeApp({
  credential: firebaseAdmin.credential.cert("firebase-auth.json"),
  databaseURL: "https://freertos-monitoring.firebaseio.com"
});
//var dataRef = firebaseAdmin.database().ref("Data");
var dataRef = firebaseAdmin.database().ref("Data");

var APP_ID = "amzn1.ask.skill.c5397674-4c85-4561-bce8-2afb2ee984af";

exports.handler = function(event, context, callback) {
    var alexa = Alexa.handler(event, context);
    alexa.APP_ID = APP_ID;
    // To enable string internationalization (i18n) features, set a resources object.
    alexa.resources = languageStrings;
    alexa.registerHandlers(handlers);
    alexa.execute();
};

var handlers = {
    //Use LaunchRequest, instead of NewSession if you want to use the one-shot model
    // Alexa, ask [my-skill-invocation-name] to (do something)...
    'LaunchRequest': function () {
        this.attributes['speechOutput'] = this.t("WELCOME_MESSAGE", this.t("SKILL_NAME"));
        // If the user either does not reply to the welcome message or says something that is not
        // understood, they will be prompted again with this text.
        this.attributes['repromptSpeech'] = this.t("WELCOME_REPROMT");
        this.emit(':ask', this.attributes['speechOutput'], this.attributes['repromptSpeech'])
    },
    'QueryIntent': function () {
        var speechOutput
        var itemSlot = this.event.request.intent.slots.Item;
        var itemName; // name of dataRequested
        if (itemSlot && itemSlot.value) {
            itemName = itemSlot.value.toLowerCase();
        }
        var cardTitle = this.t("DISPLAY_CARD_TITLE", this.t("SKILL_NAME"), itemName);

        // Call to FireBase for data object
        dataRef.once('value', function(snapshot) {
            var firebaseData = snapshot.val();

            var parsedItemName = parseSimilar(itemName);

            // Button statement for firebase object
            switch(parsedItemName){
                case "temperature":
                    speechOutput = "The board's " + itemName + " sensor reads: " + firebaseData.temp + " degrees fahrenheit.";
                    break;
                case "light":
                    speechOutput = "The board's " + itemName + " sensor reads: " + firebaseData.light + " percent.";
                    break;
                case "memory":
                    speechOutput = "The board is using " + firebaseData.mem.globalUsed + firebaseData.mem.mallocUsed + " out of " + firebaseData.mem.systemAvail + " total memory blocks. Malloc is using " + firebaseData.mem.mallocUsed + " blocks, global space is using " + firebaseData.mem.globalUsed + " blocks and there are " + firebaseData.mem.mallocUsed + " availble malloc blocks.";
                    break;
                case "button":
                    speechOutput = "The button press status is as follows: Button 1: " + firebaseData.sw[0] + ", Button 2: " + firebaseData.sw[1] + ", Button 3: " + firebaseData.sw[2] + ", and Button 4: " + firebaseData.sw[3] + ". ";
                    break;
                case "task":
                    speechOutput = "The top three tasks are: " + firebaseData.task[0].name + ". with " + firebaseData.task[0].percent + " percent, " + firebaseData.task[1].name + ". with " + firebaseData.task[1].percent + " percent, and " + firebaseData.task[2].name + ". with " + firebaseData.task[2].percent + " percent. " 
                    break;
                case "accelerometer":
                        speechOutput = "The accelerometer reads: X: " + firebaseData.x + ". Y: " + firebaseData.y + ". and, Z: " + firebaseData.z + ". ";
                        break;
                case "last update time":
                        speechOutput = "The last update was recieved on <say-as interpret-as=\"date\">" + firebaseData.date + "</say-as> at " + "<say-as interpret-as=\"time\">"+ firebaseData.time.substring(0,5) + "</say-as> . ";
                    break;
                
                default:
                    speechOutput = null;
                    break;
            }

            if (speechOutput !== null) {
                this.attributes['speechOutput'] = speechOutput;
                this.attributes['repromptSpeech'] = this.t("RECIPE_REPEAT_MESSAGE");
                this.emit(':askWithCard', this.attributes['speechOutput'], this.attributes['repromptSpeech'], cardTitle, speechOutput);
            } else {
                var speechOutput = this.t("RECIPE_NOT_FOUND_MESSAGE");
                var repromptSpeech = this.t("RECIPE_NOT_FOUND_REPROMPT");
                if (itemName) {
                    speechOutput += this.t("RECIPE_NOT_FOUND_WITH_ITEM_NAME", itemName);
                } else {
                    speechOutput += this.t("RECIPE_NOT_FOUND_WITHOUT_ITEM_NAME");
                }
                speechOutput += repromptSpeech;

                this.attributes['speechOutput'] = speechOutput;
                this.attributes['repromptSpeech'] = repromptSpeech;

                this.emit(':ask', speechOutput, repromptSpeech);
            }
        }.bind(this));

    },
    'AMAZON.HelpIntent': function () {
        this.attributes['speechOutput'] = this.t("HELP_MESSAGE");
        this.attributes['repromptSpeech'] = this.t("HELP_REPROMT");
        this.emit(':ask', this.attributes['speechOutput'], this.attributes['repromptSpeech'])
    },
    'AMAZON.RepeatIntent': function () {
        this.emit(':ask', this.attributes['speechOutput'], this.attributes['repromptSpeech'])
    },
    'AMAZON.StopIntent': function () {
        this.emit('SessionEndedRequest');
    },
    'AMAZON.CancelIntent': function () {
        this.emit('SessionEndedRequest');
    },
    'SessionEndedRequest':function () {
        this.emit(':tell', this.t("STOP_MESSAGE", this.t("SKILL_NAME")));
    }
};

var languageStrings = {
    "en-US": {
        "translation": {
            "SKILL_NAME" : "MicroWatch",
            "WELCOME_MESSAGE": "Welcome to %s. You can request SJOne board info by saying things like, 'what is the current temperature', or 'what is the current light value'... Now, what can I help you with?",
            "WELCOME_REPROMT": "For a list of accepted commands, just say 'help'.",
            "DISPLAY_CARD_TITLE": "%s  - Value for %s.",
            "HELP_MESSAGE": "You can say things like, 'what is the current temperature'... Or you can invoke the skill directly by saying 'Ask MicroWatch for the current temperature'... Now, what can I help you with?",
            "HELP_REPROMT": "You can say things like, 'what is the current temperature', or you can say exit... Now, what can I help you with?",
            "STOP_MESSAGE": "Thanks for using %s, Goodbye!",
            "RECIPE_REPEAT_MESSAGE": "Try saying repeat.",
            "RECIPE_NOT_FOUND_MESSAGE": "I'm sorry, I currently can't monitor ",
            "RECIPE_NOT_FOUND_WITH_ITEM_NAME": "the %s. ",
            "RECIPE_NOT_FOUND_WITHOUT_ITEM_NAME": "that parameter. ",
            "RECIPE_NOT_FOUND_REPROMPT": "What else can I help with?"
        }
    }
};

function parseSimilar(item){
    switch(item){
        case "task usage":
            item = "task";
            break;
        case "cpu usage":
            item = "task";
            break;
        case "cpu":
            item = "task";
            break;
        case "last update":
            item = "last update time";
            break;
        case "accel":
            item = "accelerometer";
            break;
        default:
            break;
    }
    return item
}
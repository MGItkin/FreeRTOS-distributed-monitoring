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

        // var recipes = this.t("RECIPES");
        // var recipe = recipes[itemName];

        // Call to FireBase for requested item
        dataRef.once('value', function(snapshot) {
            var firebaseData = snapshot.val();

            // Switch statement for firebase object
            switch(itemName){
                case "temperature":
                    speechOutput = "The board's " + itemName + " is " + firebaseData.temp + " degrees fahrenheit.";
                    break;
                case "light":
                    speechOutput = "The board's " + itemName + " is " + firebaseData.light + " percent.";
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
            "WELCOME_MESSAGE": "Welcome to %s. You can request SJOne board info by saying things like, 'what is the current temperature?'... Now, what can I help you with?",
            "WELCOME_REPROMT": "For a list of accepted commands, just say 'help'.",
            "DISPLAY_CARD_TITLE": "%s  - Value for %s.",
            "HELP_MESSAGE": "You can ask questions such as, what\'s the recipe, or, you can say exit...Now, what can I help you with?",
            "HELP_REPROMT": "You can say things like, what\'s the recipe, or you can say exit...Now, what can I help you with?",
            "STOP_MESSAGE": "Thanks for using %s, Goodbye!",
            "RECIPE_REPEAT_MESSAGE": "Try saying repeat.",
            "RECIPE_NOT_FOUND_MESSAGE": "I\'m sorry, I currently can't monitor ",
            "RECIPE_NOT_FOUND_WITH_ITEM_NAME": "the %s. ",
            "RECIPE_NOT_FOUND_WITHOUT_ITEM_NAME": "that parameter. ",
            "RECIPE_NOT_FOUND_REPROMPT": "What else can I help with?"
        }
    }
};
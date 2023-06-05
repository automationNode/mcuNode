module.exports = function (RED) {
  "use strict";
  var fetch = require("node-fetch");

  function digitalRead(config) {
    RED.nodes.createNode(this, config);
    var node = this;
    node.on("input", function (msg) {
      msg.payload = msg.payload.toLowerCase();
      node.send(msg);
      node.send({payload: fetch});
    });
  }
  RED.nodes.registerType("digitalRead", digitalRead);
};

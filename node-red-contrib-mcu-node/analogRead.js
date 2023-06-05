module.exports = function (RED) {
  "use strict";
  var fetch = require("node-fetch");

  function analogRead(config) {
    RED.nodes.createNode(this, config);
    var node = this;
    node.on("input", async function (msg) {
      console.log("-->Input:", config);
      if (msg.payload.executed == true || msg.payload.start == true) {
        try {
          let content = {
            execute: true,
            instruction: {
              action: "analogRead",
              parameters: [config.gpio],
            },
          };
          this.status({ fill: "yellow", shape: "dot", text: "Executing..." });
          let response = await fetch(`http://${config.address}/data`, {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(content),
          });
          response = await response.json();
          console.log("-->Output:", response);
          if (response.return) {
            this.status({ fill: "green", shape: "dot", text: "Executed" });
            node.send({ payload: response.instruction });
          }
        } catch (err) {
          this.status({ fill: "red", shape: "dot", text: "Not executed" });
          node.send({ payload: err });
        }
      }
    });
  }
  RED.nodes.registerType("analogRead", analogRead);
};

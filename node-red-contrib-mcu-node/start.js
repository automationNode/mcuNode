module.exports = function (RED) {
  "use strict";

  function start(config) {
    RED.nodes.createNode(this, config);
    var node = this;
    node.on("input", function (msg) {
      node.send(msg);
      node.send({
        payload: {
          start: true,
        },
      });
    });
  }
  RED.nodes.registerType("start", start);
};

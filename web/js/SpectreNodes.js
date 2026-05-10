import { app } from "/scripts/app.js";

app.registerExtension({
    name: "SpectreNodes.SeedNode",
    beforeRegisterNodeDef(nodeType, nodeData) {
        if (nodeData.name === "SpectreNodesSeedXoshiro256**") {
            const orig = nodeType.prototype.onExecuted;
            nodeType.prototype.onExecuted = function(data) {
                orig?.apply(this, arguments);
                const widget = this.widgets?.find(w => w.name === "seed_value");
                if (widget && data?.seed_value?.[0] !== undefined) {
                    widget.value = data.seed_value[0];
                }
            };
        }
    }
});
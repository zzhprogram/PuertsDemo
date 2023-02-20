"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.UIManager = void 0;
const puerts_1 = require("puerts");
const UE = require("ue");
class UIManager {
    static _instance;
    static get instance() {
        if (!this._instance) {
            this._instance = new UIManager();
        }
        return this._instance;
    }
    constructor() {
        this._world = puerts_1.argv.getByName("GameInstance").GetWorld();
    }
    _uiArray = new Map();
    _world = null;
    showUI() {
        let widgetClass = UE.Class.Load("/Game/StarterContent/TestWidgetBlueprint.TestWidgetBlueprint_C");
        const widget = UE.UMGManager.CreateWidget(this._world, widgetClass);
        this._uiArray.set(0, widget);
        widget.AddToViewport(0);
        widget.Button1.OnClicked.Add(this._onButtonClicked.bind(this));
    }
    hideUI() {
    }
    clearUI() {
        const widget = this._uiArray.get(0);
        widget.RemoveFromViewport();
        widget.Button1.OnClicked.Clear();
        this._uiArray.clear();
    }
    _onButtonClicked() {
        this.hideUI();
        this.clearUI();
        setTimeout(() => {
            UE.GameplayStatics.OpenLevel(this._world, "ChlidLevel");
        }, 1);
    }
}
exports.UIManager = UIManager;
//# sourceMappingURL=UIManager.js.map
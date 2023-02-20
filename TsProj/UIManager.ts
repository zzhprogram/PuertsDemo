import * as puerts from "puerts";
import * as UE from "ue";

export class UIManager {
    private static _instance: UIManager;
    public static get instance(): UIManager {
        if (!this._instance) {
            this._instance = new UIManager();
        }
        return this._instance;
    }

    public constructor() {
        this._world = (puerts.argv.getByName("GameInstance") as UE.GameInstance).GetWorld() as UE.World;
    }

    private _uiArray: Map<number, UE.UserWidget> = new Map();
    private _world: UE.World = null;

    // 在manager中创建并展示UI。
    public showUI() {
        let widgetClass = UE.Class.Load("/Game/StarterContent/TestWidgetBlueprint.TestWidgetBlueprint_C");

        let widget = UE.UMGManager.CreateWidget(this._world, widgetClass) as UE.Game.StarterContent.TestWidgetBlueprint.TestWidgetBlueprint_C;

        // 将UI添加到Map中，方便后续管理。
        this._uiArray.set(0, widget);

        widget.AddToViewport(0);

        // 绑定按钮点击事件。
        widget.Button1.OnClicked.Add(this._onButtonClicked.bind(this));
    }

    public hideUI() {

    }

    public clearUI() {
        const widget = this._uiArray.get(0) as UE.Game.StarterContent.TestWidgetBlueprint.TestWidgetBlueprint_C;
        widget.RemoveFromViewport();
        widget.Button1.OnClicked.Clear();
        this._uiArray.clear();
    }

    // 切换关卡，解绑按钮事件，清理容器UI。
    private _onButtonClicked() {
        this.clearUI();
        UE.GameplayStatics.OpenLevel(this._world, "ChlidLevel");
    }
}

export class DelayAction {
    private _action: ()=>void;
    private _timer: number = null;
    private _delay:number;

    constructor(delay:number, action: ()=>void) {
        this._action = action;
        this._delay = delay;
    }

    start(){
        this.stop();
        this._timer = setTimeout(this.ExeAction, this._delay);
    }

    stop(){
        if(this._timer != null){
            clearTimeout(this._timer);
        }
        this._timer = null;
    }

    ExeAction(){
        this._action();
        this._timer = null;
    }
}
Class Miner:public BaseGameEntity
{
private:
    State* CurrentState;
    location_type  Location;
    int GoldCarried;
    int MoneyInBank;
    int Thirst;
    int Fatigue;
public:
    Miner(int ID);
    void Update()
    {
        Thirst+=1;
        if(CurrentState)
        {
            CurrentState->Execute(this);
        }
    }
    void ChangeState(State* NewState);
}

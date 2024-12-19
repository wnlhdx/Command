class BaseGameEntity
{
private:
    int ID;
    static int validId;
    void SetID(int val);
public:
    BaseGameEntity(int id)
    {
        setID(id);
    }
    virtual BaseGameEntity(){}
    virtual void Update{}=0;
    int ID()const{return ID;}
};

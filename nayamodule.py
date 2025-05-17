import pandas as pd
from sklearn.tree import DecisionTreeRegressor
import joblib
import os

# Setup / Load Existing Model or Data
MODEL_FILE = "fan_model.pkl"
DATA_FILE = "fan_data.csv"

# Load existing data or create new
if os.path.exists(DATA_FILE):
    df = pd.read_csv(DATA_FILE)
else:
    df = pd.DataFrame(columns=["temperature", "user", "fan_speed"])

# Convert user to numeric
def preprocess(df):
    df_copy = df.copy()
    df_copy["user_id"] = df_copy["user"].astype("category").cat.codes
    return df_copy

# --------- Train Model ----------
def train_model():
    df_train = preprocess(df)
    if len(df_train) < 3:
        print("Not enough data to train.")
        return None
    X = df_train[["temperature", "user_id"]]
    y = df_train["fan_speed"]
    model = DecisionTreeRegressor()
    model.fit(X, y)
    joblib.dump((model, df_train), MODEL_FILE)
    print(" Model trained and saved.")
    return model

# --------- Predict Fan Speed ----------
def predict_fan_speed(temp, user):
    if not os.path.exists(MODEL_FILE):
        print("No model yet. Add more data.")
        return None
    model, df_train = joblib.load(MODEL_FILE)
    user_map = dict(zip(df_train["user"], df_train["user_id"]))
    if user not in user_map:
        print("New user! Add some data first.")
        return None
    user_id = user_map[user]
    X = [[temp, user_id]]
    return model.predict(X)[0]

# add_data(temp, user, speed):
    global df
    df = pd.concat([df, pd.DataFrame([[temp, user, speed]], columns=df.columns)], ignore_index=True)
    df.to_csv(DATA_FILE, index=False)
    train_model()
    print("New data added and model retrained.")

# --------- Demo / CLI ---------
while True:
    print("\n--- Fan Behavior System ---")
    choice = input("1. Predict Fan Speed\n2. Add Training Data\n3. Exit\n> ")

    if choice == "1":
        temp = float(input("Enter temperature: "))
        user = input("Enter user name: ")
        speed = predict_fan_speed(temp, user)
        if speed is not None:
            print(f" Predicted Fan Speed: {round(speed)}")
    elif choice == "2":
        temp = float(input("Temperature: "))
        user = input("User: ")
        speed = int(input("Actual Fan Speed used: "))
        add_data(temp, user, speed)
    elif choice == "3":
        break
    else:
        print("Invalid option.")

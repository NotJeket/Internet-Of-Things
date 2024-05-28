from flask import Flask, request, jsonify
from flask_jwt_extended import JWTManager, create_access_token, jwt_required, get_jwt_identity

app = Flask(__name__)
app.config['JWT_SECRET_KEY'] = 'hoi-welocom-to-temshop'  # secret key
jwt = JWTManager(app)

# user data
users = {
    "guest": {"password": "guest", "role": "guest"},
    "admin": {"password": "admin", "role": "admin"},
    "gm": {"password": "gm", "role": "owner"},
    
}

# dictionary for JWT store
jwt_store = {}

# Login route
@app.route('/auth', methods=['POST'])
def login():
    data = request.json
    username = data.get('username')
    password = data.get('password')

    user = users.get(username)
    if user and user['password'] == password:
        access_token = create_access_token(identity={"username": username, "role": user['role']})
        jwt_store[access_token] = {"username": username, "role": user['role']}
        return jsonify(access_token=access_token), 200
    return jsonify({"msg": "Invalid credentials"}), 401

# check route for validty
@app.route('/auth/jwtStore', methods=['GET'])
@jwt_required()
def check_token():
    current_user = get_jwt_identity()
    return jsonify(current_user), 200

# logout for invality
@app.route('/auth/jwtStore', methods=['DELETE'])
@jwt_required()
def logout():
    current_user = get_jwt_identity()
    jwt_token = request.headers.get('Authorization').split()[1]
    if jwt_token in jwt_store:
        del jwt_store[jwt_token]
        return jsonify({"msg": "Token has been revoked"}), 200
    return jsonify({"msg": "Token not found"}), 404

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000)

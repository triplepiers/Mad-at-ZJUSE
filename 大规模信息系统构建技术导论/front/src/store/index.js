import { createStore } from "vuex"

const actions = {

}

const mutations = {
    UPDATE(state, neoTables) {
        state.tables = neoTables
    }
}

const state = {
    tables: []
}

export default createStore({
    actions,
    mutations,
    state
})